#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "RevoInternal/log.hpp"
#include <sstream>

using namespace revointernal;

TEST_CASE("Logging - Set and get log level", "[logging]") {
    setLogLevel(LogLevel::Debug);
    REQUIRE(getLogLevel() == LogLevel::Debug);

    setLogLevel(LogLevel::Error);
    REQUIRE(getLogLevel() == LogLevel::Error);
}

TEST_CASE("Logging - Custom log function", "[logging]") {
    std::ostringstream output;
    LogLevel capturedLevel = LogLevel::Off;
    std::string capturedMessage;

    setLogFunc([&](LogLevel level, const std::string& message) {
        capturedLevel = level;
        capturedMessage = message;
        output << message;
    });

    setLogLevel(LogLevel::Trace);

    SECTION("Logs at specified level") {
        log(LogLevel::Warning, "Test warning: %d", 42);
        REQUIRE(capturedLevel == LogLevel::Warning);
        REQUIRE(capturedMessage == "Test warning: 42");
    }

    SECTION("Filters messages below log level") {
        setLogLevel(LogLevel::Warning);
        capturedMessage.clear();

        log(LogLevel::Debug, "Should not appear");
        REQUIRE(capturedMessage.empty());

        log(LogLevel::Error, "Should appear");
        REQUIRE(capturedMessage == "Should appear");
    }

    // Cleanup
    setLogFunc(nullptr);
    setLogLevel(LogLevel::Info);
}

TEST_CASE("Logging - Macros work correctly", "[logging]") {
    std::string lastMessage;
    setLogFunc([&](LogLevel level, const std::string& message) {
        lastMessage = message;
    });

    setLogLevel(LogLevel::Trace);

    LOG_INFO("Info message: %s", "test");
    REQUIRE(lastMessage == "Info message: test");

    LOG_ERROR("Error message: %d", 123);
    REQUIRE(lastMessage == "Error message: 123");

    // Cleanup
    setLogFunc(nullptr);
    setLogLevel(LogLevel::Info);
}
