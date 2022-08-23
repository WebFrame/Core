#ifndef _Project_home_with_render_route
#define _Project_home_with_render_route

#include <webframe/webframe.hpp>

webframe::router home_with_render(webframe::webframe& app) {
    init_routes(home)
        .route("/home", [&app]() {
			return app.render("template.html");
        });
    return home;
}
#endif