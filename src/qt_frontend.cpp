#include "qt_frontend.h"

#include "game.h"
#include "input.h"

#include <QApplication>
#include <QColor>
#include <QCloseEvent>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QLabel>
#include <QMetaObject>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <exception>
#include <iostream>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>

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
        textFormat_.setForeground(Qt::black);
        textFormat_.setBackground(Qt::white);
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
            QTextCursor cursor = output_->textCursor();
            cursor.insertText(text, textFormat_);
            output_->setTextCursor(cursor);
            output_->verticalScrollBar()->setValue(output_->verticalScrollBar()->maximum());
        }, Qt::QueuedConnection);
    }

    void setTextStyle(const QColor& foreground, const QColor& background,
                      bool bold, bool underline) {
        QMetaObject::invokeMethod(output_, [this, foreground, background, bold, underline] {
            textFormat_.setForeground(foreground);
            textFormat_.setBackground(background);
            textFormat_.setFontWeight(bold ? QFont::Bold : QFont::Normal);
            textFormat_.setFontUnderline(underline);
        }, Qt::QueuedConnection);
    }

    void clearOutput() {
        QMetaObject::invokeMethod(output_, [this] {
            output_->clear();
            output_->moveCursor(QTextCursor::Start);
        }, Qt::QueuedConnection);
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
    QTextCharFormat textFormat_;
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
    static QColor xtermColor(int index, bool foreground) {
        static const QColor baseColors[] = {
            QColor(0, 0, 0), QColor(128, 0, 0), QColor(0, 128, 0), QColor(128, 128, 0),
            QColor(0, 0, 128), QColor(128, 0, 128), QColor(0, 128, 128), QColor(192, 192, 192),
            QColor(128, 128, 128), QColor(255, 0, 0), QColor(0, 255, 0), QColor(255, 255, 0),
            QColor(0, 0, 255), QColor(255, 0, 255), QColor(0, 255, 255), QColor(255, 255, 255)
        };
        index = std::clamp(index, 0, 255);
        QColor color;
        if (index < 16) {
            color = baseColors[index];
        } else if (index < 232) {
            const int cube = index - 16;
            const auto component = [](int value) { return value == 0 ? 0 : 55 + value * 40; };
            color = QColor(component(cube / 36), component((cube / 6) % 6),
                           component(cube % 6));
        } else {
            const int gray = 8 + (index - 232) * 10;
            color = QColor(gray, gray, gray);
        }

        if (foreground) {
            if (index == 15) return QColor(20, 20, 20);
            if (color.lightness() > 100) color = color.darker(155);
        }
        return color;
    }

    void applySgr(const std::string& parameters) {
        std::vector<int> values;
        int value = 0;
        bool hasValue = false;
        for (char ch : parameters) {
            if (ch >= '0' && ch <= '9') {
                value = value * 10 + (ch - '0');
                hasValue = true;
            } else if (ch == ';') {
                values.push_back(hasValue ? value : 0);
                value = 0;
                hasValue = false;
            }
        }
        values.push_back(hasValue ? value : 0);

        for (std::size_t i = 0; i < values.size(); ++i) {
            const int code = values[i];
            if (code == 0) {
                foreground_ = QColor(0, 0, 0);
                background_ = QColor(255, 255, 255);
                bold_ = false;
                underline_ = false;
                dim_ = false;
            } else if (code == 1) {
                bold_ = true;
            } else if (code == 2) {
                dim_ = true;
            } else if (code == 4) {
                underline_ = true;
            } else if (code == 22) {
                bold_ = false;
                dim_ = false;
            } else if (code == 24) {
                underline_ = false;
            } else if (code == 39) {
                foreground_ = QColor(0, 0, 0);
            } else if (code == 49) {
                background_ = QColor(255, 255, 255);
            } else if (code >= 30 && code <= 37) {
                foreground_ = xtermColor(code - 30, true);
            } else if (code >= 90 && code <= 97) {
                foreground_ = xtermColor(code - 90 + 8, true);
            } else if (code >= 40 && code <= 47) {
                background_ = xtermColor(code - 40, false);
            } else if (code >= 100 && code <= 107) {
                background_ = xtermColor(code - 100 + 8, false);
            } else if ((code == 38 || code == 48) && i + 1 < values.size()) {
                QColor color;
                const bool isForeground = code == 38;
                if (values[i + 1] == 5 && i + 2 < values.size()) {
                    color = xtermColor(values[i + 2], isForeground);
                    i += 2;
                } else if (values[i + 1] == 2 && i + 4 < values.size()) {
                    color = QColor(std::clamp(values[i + 2], 0, 255),
                                   std::clamp(values[i + 3], 0, 255),
                                   std::clamp(values[i + 4], 0, 255));
                    i += 4;
                }
                if (color.isValid()) {
                    if (isForeground) foreground_ = color;
                    else background_ = color;
                }
            }
        }
        QColor foreground = foreground_;
        if (dim_) foreground = foreground.darker(125);
        window_.setTextStyle(foreground, background_, bold_, underline_);
    }

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
                } else if (ch == 'm') {
                    applySgr(ansiParams_);
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
    QColor foreground_{0, 0, 0};
    QColor background_{255, 255, 255};
    bool bold_ = false;
    bool underline_ = false;
    bool dim_ = false;
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
