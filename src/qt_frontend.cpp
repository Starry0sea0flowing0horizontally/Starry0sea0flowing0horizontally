#include "qt_frontend.h"

#include "game.h"
#include "input.h"

#include <QApplication>
#include <QCloseEvent>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QLabel>
#include <QMetaObject>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <exception>
#include <QFontMetrics>
#include <iostream>
#include <streambuf>
#include <string>
#include <thread>

namespace game {
namespace {

class GameOutput final : public QPlainTextEdit {
public:
    using QPlainTextEdit::QPlainTextEdit;

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            const QFontMetrics metrics(font());
            const int charWidth = metrics.horizontalAdvance(QLatin1Char('M'));
            const QPoint position = event->position().toPoint();
            const int margin = static_cast<int>(document()->documentMargin());
            const int textX = position.x() - margin;
            const int x = charWidth > 0 && textX >= 0 ? textX / charWidth : -1;
            const int y = firstVisibleBlock().blockNumber() +
                          position.y() / std::max(1, metrics.lineSpacing());
            if (x >= 0 && y >= 0) input::pushMouseClick(x, y);
            event->accept();
            return;
        }
        QPlainTextEdit::mousePressEvent(event);
    }
};

class GameWindow final : public QWidget {
public:
    GameWindow() : output_(new GameOutput(this)) {
        setWindowTitle(QStringLiteral("花至荼蘼"));
        resize(1000, 760);

        auto* layout = new QVBoxLayout(this);
        auto* help = new QLabel(
            QStringLiteral("方向键移动/选择，回车确认，空格推进，Esc 打开菜单，数字键快速选择"),
            this);
        output_->setReadOnly(true);
        output_->setUndoRedoEnabled(false);
        output_->setLineWrapMode(QPlainTextEdit::NoWrap);
        output_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        output_->setFocusPolicy(Qt::NoFocus);
        output_->setStyleSheet(
            QStringLiteral("QPlainTextEdit { background: #ffffff; color: #000000; "
                           "selection-background-color: #cce5ff; }"));
        setStyleSheet(QStringLiteral("QWidget { background: #ffffff; color: #000000; }"));
        layout->addWidget(help);
        layout->addWidget(output_);
        setFocusPolicy(Qt::StrongFocus);
    }

    void appendText(const QString& text) {
        QMetaObject::invokeMethod(output_, [this, text] {
            output_->moveCursor(QTextCursor::End);
            output_->insertPlainText(text);
            output_->verticalScrollBar()->setValue(output_->verticalScrollBar()->maximum());
        }, Qt::QueuedConnection);
    }

    void clearOutput() {
        QMetaObject::invokeMethod(output_, [this] { output_->clear(); }, Qt::QueuedConnection);
    }

    void eraseLine() {
        QMetaObject::invokeMethod(output_, [this] {
            QTextCursor cursor = output_->textCursor();
            cursor.select(QTextCursor::LineUnderCursor);
            cursor.removeSelectedText();
            output_->setTextCursor(cursor);
        }, Qt::QueuedConnection);
    }

    void moveCursorUp() {
        QMetaObject::invokeMethod(output_, [this] {
            QTextCursor cursor = output_->textCursor();
            cursor.movePosition(QTextCursor::Up);
            output_->setTextCursor(cursor);
        }, Qt::QueuedConnection);
    }

protected:
    void keyPressEvent(QKeyEvent* event) override {
        int key = input::KEY_NONE;
        switch (event->key()) {
            case Qt::Key_Up: key = input::KEY_UP; break;
            case Qt::Key_Down: key = input::KEY_DOWN; break;
            case Qt::Key_Left: key = input::KEY_LEFT; break;
            case Qt::Key_Right: key = input::KEY_RIGHT; break;
            case Qt::Key_Return:
            case Qt::Key_Enter: key = input::KEY_ENTER; break;
            case Qt::Key_Escape: key = input::KEY_ESC; break;
            case Qt::Key_Backspace: key = input::KEY_BACKSPACE; break;
            case Qt::Key_Tab: key = input::KEY_TAB; break;
            case Qt::Key_Space: key = input::KEY_SPACE; break;
            default: {
                const QString text = event->text();
                if (text.size() == 1 && text.at(0).unicode() < 0x80)
                    key = text.at(0).toLatin1();
                break;
            }
        }
        if (key != input::KEY_NONE) input::pushKey(key);
        else QWidget::keyPressEvent(event);
    }

    void closeEvent(QCloseEvent* event) override {
        input::requestQuit();
        QWidget::closeEvent(event);
    }

private:
    GameOutput* output_;
};

class GuiOutputBuffer final : public std::streambuf {
public:
    explicit GuiOutputBuffer(GameWindow& window) : window_(window) {}

protected:
    int_type overflow(int_type value) override {
        if (traits_type::eq_int_type(value, traits_type::eof())) return traits_type::not_eof(value);
        consume(traits_type::to_char_type(value));
        return value;
    }

    std::streamsize xsputn(const char* data, std::streamsize size) override {
        for (std::streamsize i = 0; i < size; ++i) consume(data[i]);
        return size;
    }

    int sync() override {
        flushText(false);
        return 0;
    }

private:
    void consume(char ch) {
        if (ansiState_ == 1) {
            ansiState_ = (ch == '[') ? 2 : 0;
            return;
        }
        if (ansiState_ == 2) {
            if (ch >= 0x40 && ch <= 0x7e) {
                if (ch == 'J' && ansiParams_.find('2') != std::string::npos) {
                    flushText(true);
                    window_.clearOutput();
                } else if (ch == 'F') {
                    flushText(true);
                    window_.moveCursorUp();
                } else if (ch == 'K') {
                    flushText(true);
                    window_.eraseLine();
                }
                ansiParams_.clear();
                ansiState_ = 0;
            } else {
                ansiParams_.push_back(ch);
            }
            return;
        }
        if (ch == '\033') {
            flushText(true);
            ansiState_ = 1;
            return;
        }
        text_.push_back(ch);
    }

    void flushText(bool force) {
        std::size_t complete = text_.size();
        if (!force) {
            std::size_t start = complete;
            while (start > 0 &&
                   (static_cast<unsigned char>(text_[start - 1]) & 0xc0) == 0x80) {
                --start;
            }
            if (start < complete) {
                if (start == 0) complete = 0;
                else {
                    const unsigned char lead = static_cast<unsigned char>(text_[start - 1]);
                    const std::size_t expected = (lead & 0xf8) == 0xf0 ? 4 :
                                                 (lead & 0xf0) == 0xe0 ? 3 :
                                                 (lead & 0xe0) == 0xc0 ? 2 : 1;
                    if (complete - (start - 1) < expected) complete = start - 1;
                }
            } else if (complete > 0) {
                const unsigned char last = static_cast<unsigned char>(text_.back());
                if ((last & 0x80) != 0) {
                    const std::size_t expected = (last & 0xf8) == 0xf0 ? 4 :
                                                 (last & 0xf0) == 0xe0 ? 3 :
                                                 (last & 0xe0) == 0xc0 ? 2 : 1;
                    if (expected > 1) complete = text_.size() - 1;
                }
            }
        }
        if (complete == 0) return;
        window_.appendText(QString::fromUtf8(text_.data(), static_cast<int>(complete)));
        text_.erase(0, complete);
    }

    GameWindow& window_;
    std::string text_;
    std::string ansiParams_;
    int ansiState_ = 0;
};

}  // namespace

int runQtFrontend(int argc, char* argv[]) {
    QApplication app(argc, argv);
    GameWindow window;
    GuiOutputBuffer output(window);
    std::streambuf* previousOutput = std::cout.rdbuf(&output);

    window.show();
    std::thread gameThread([&app, &output] {
        try {
            Game game;
            game.run();
        } catch (const input::InputCancelled&) {
        } catch (const std::exception& error) {
            std::cout << "\n游戏异常终止：" << error.what() << '\n';
        }
        QMetaObject::invokeMethod(&app, [&app] { app.quit(); }, Qt::QueuedConnection);
    });

    const int result = app.exec();
    input::requestQuit();
    gameThread.join();
    std::cout.rdbuf(previousOutput);
    return result;
}

}  // namespace game
