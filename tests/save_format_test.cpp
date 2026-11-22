#include "save.h"

#include <archive.h>
#include <archive_entry.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

namespace {

bool validArchive(const std::string& bytes) {
    struct archive* reader = archive_read_new();
    if (!reader) return false;
    bool ok = archive_read_support_filter_gzip(reader) == ARCHIVE_OK &&
              archive_read_support_format_tar(reader) == ARCHIVE_OK &&
              archive_read_open_memory(reader, bytes.data(), bytes.size()) == ARCHIVE_OK;
    struct archive_entry* entry = nullptr;
    if (ok) {
        const char* pathname = nullptr;
        if (archive_read_next_header(reader, &entry) == ARCHIVE_OK) {
            pathname = archive_entry_pathname(entry);
        }
        ok = pathname != nullptr &&
             archive_entry_filetype(entry) == AE_IFREG &&
             std::string(pathname) == "save";
    }
    if (ok) {
        char buffer[8192];
        while (true) {
            la_ssize_t n = archive_read_data(reader, buffer, sizeof(buffer));
            if (n == 0) break;
            if (n < 0) {
                ok = false;
                break;
            }
        }
    }
    if (ok) ok = archive_read_next_header(reader, &entry) == ARCHIVE_EOF;
    if (archive_read_close(reader) != ARCHIVE_OK) ok = false;
    if (archive_read_free(reader) != ARCHIVE_OK) ok = false;
    return ok;
}

bool check(bool condition, const char* message) {
    if (condition) return true;
    std::cerr << message << '\n';
    return false;
}

}  // namespace

int main() {
    const std::filesystem::path dir =
        std::filesystem::temp_directory_path() /
        ("huazhitumi-save-test-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    struct TempDirectory {
        std::filesystem::path path;
        ~TempDirectory() {
            std::error_code ec;
            std::filesystem::remove_all(path, ec);
        }
    } cleanup{dir};

    game::SaveManager saves(dir.string());
    game::SaveData expected;
    expected.pc = 27;
    expected.resumeLabel = "prologue_start";
    expected.resumeOffset = 4;
    expected.flags["choice"] = "yes";
    expected.chapter = "chapter";
    expected.sceneTitle = "scene";
    expected.timestamp = "2026-01-02 03:04:05";
    if (!check(saves.save(0, expected), "failed to save archive")) return 1;
    if (!check(saves.pathFor(0).size() >= 4 &&
               saves.pathFor(0).substr(saves.pathFor(0).size() - 4) == ".sav",
               "save extension changed")) return 1;

    std::ifstream file(saves.pathFor(0).c_str(), std::ios::in | std::ios::binary);
    std::string bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (!check(bytes.size() >= 2 &&
               static_cast<unsigned char>(bytes[0]) == 0x1F &&
               static_cast<unsigned char>(bytes[1]) == 0x8B,
               "save is not gzip-compressed")) return 1;
    if (!check(validArchive(bytes), "save is not a tar.gz archive with a suffixless member")) return 1;

    game::SaveData loaded;
    if (!check(saves.load(0, loaded), "failed to load compressed save")) return 1;
    if (!check(loaded.pc == expected.pc &&
               loaded.resumeLabel == expected.resumeLabel &&
               loaded.resumeOffset == expected.resumeOffset &&
               loaded.flags == expected.flags &&
               loaded.chapter == expected.chapter &&
               loaded.sceneTitle == expected.sceneTitle &&
               loaded.timestamp == expected.timestamp,
               "compressed save round-trip changed data")) return 1;

    {
        std::ofstream legacy(saves.pathFor(1).c_str(), std::ios::out | std::ios::binary);
        legacy << "version 1\nchapter legacy\npc 9\nscene old scene\n"
                  "timestamp 2025-01-02 03:04:05\nflag old=yes\n";
    }
    if (!check(saves.load(1, loaded) &&
               loaded.chapter == "legacy" && loaded.pc == 9 &&
               loaded.sceneTitle == "old scene" && loaded.flags["old"] == "yes",
               "legacy plaintext save is not readable")) return 1;

    {
        std::ofstream corrupt(saves.pathFor(2).c_str(), std::ios::out | std::ios::binary);
        const char damaged[] = {
            static_cast<char>(0x1F), static_cast<char>(0x8B), 'b', 'r', 'o', 'k', 'e', 'n'
        };
        corrupt.write(damaged, sizeof(damaged));
    }
    if (!check(!saves.load(2, loaded), "corrupted gzip save was accepted")) return 1;
    return 0;
}
