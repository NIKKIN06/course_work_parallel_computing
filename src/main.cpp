#include <crow.h>
#include <stdio.h>

bool check_ollama_status()
{
    return true;
}

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/health")
    ([]()
     {
        crow::json::wvalue response;

        response["server_status"] = "OK";
        response["ollama_status"] = check_ollama_status();

        return response; });

    app.port(8080).run();

    return 0;
}