#pragma once

#include <moka/moka.h>
#include <webframe/webframe.hpp>

void testCase_UnitTests (Moka::Report& report) {
	std::filebuf fileBuffer;
	fileBuffer.open ("./bin/log/buffer-UT.txt", std::ios::out);
	std::ostream nil (&fileBuffer);

    Moka::Context unit_tests ("WebFrame - unit testing", [&nil](Moka::Context& it) {
        it.describe("responser", [&nil](Moka::Context& it) {
            it.should("respond with status code 200 by default", [&nil]() {
                webframe::webframe app;
                app
                .set_logger(nil)
				.set_warner(nil)
                .set_error_logger(nil)
                .route ("/", []() { // static setup
                    return "";
                });
                
                auto r = (*app.get_routes().begin()).second.call("1.1", "", webframe::path_vars()).to_string();
                must_contain("HTTP/1.1 200 OK", r, "The server did not respond with 200 OK by default.");
            });
            it.should("respond with status code 500 when set to", [&nil]() {
                webframe::webframe app;
                app
                .set_logger(nil)
				.set_warner(nil)
                .set_error_logger(nil)
                .route ("/", []() { // static setup
                    return webframe::response (webframe::status_line ("500"), {{"Content-Type", "text/html; charset=utf-8"}}, "");
                });
                
                auto r = (*app.get_routes().begin()).second.call("1.1", "", webframe::path_vars()).to_string();
                
                must_contain("HTTP/1.1 500 Internal Server Error", r, "The server did not respond with the status code.");
            });
            
            it.should("respond with the correct html", [&nil]() {
                const std::string text = "sample";
                webframe::webframe app;
                app
                .set_logger(nil)
				.set_warner(nil)
                .set_error_logger(nil)
                .route ("/", [text]() { // static setup
                    return text;
                });
                
                auto r = (*app.get_routes().begin()).second.call("1.1", "", webframe::path_vars()).to_string();
                
                must_contain(text, r, "The server did not respond with the correct body.");
            });

            it.should("respond with the given path variable value", [&nil]() {
                const std::string username = "sample username";
                webframe::webframe app;
                app
                .set_logger(nil)
				.set_warner(nil)
                .set_error_logger(nil)
                .route ("/{.*}", [](std::string username) { // static setup
                    return webframe::response (webframe::status_line ("1.1", "201"), {{"Content-Type", "text/html; charset=utf-8"}}, username);
                });
                auto params = webframe::path_vars();
                params += {username, "string"};

                auto r = (*app.get_routes().begin()).second.call("1.1", "", params).to_string();
                
                must_contain(username, r, "The server did not respond with the correct body when path vavriable is passed.");
            });
            it.should("respond with a custom header", [&nil]() {
                const std::string username = "sample username", testing_header="testing header";
            
                webframe::webframe app;
                app
                .set_logger(nil)
				.set_warner(nil)
                .set_error_logger(nil)
                .route ("/{.*}", [testing_header](std::string username) { // static setup
                    return webframe::response (webframe::status_line ("1.1", "201"), {{"Custom-header", testing_header}, {"Content-Type", "text/html; charset=utf-8"}}, username);
                });
                auto params = webframe::path_vars();
                params += {username, "string"};
                
                auto r = (*app.get_routes().begin()).second.call("1.1", "", params).to_string();
                
                must_contain("Custom-header: " + testing_header, r, "The server did not include the custom header to the response.");
            });
        });
    });
	unit_tests.test(report);
}