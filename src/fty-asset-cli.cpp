/*  =========================================================================
    Copyright (C) 2014 - 2020 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

#include <asset/conversion/json.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/jsonserializer.h>
#include <filesystem>
#include <fty_asset_dto.h>
#include <fty_common_messagebus.h>
#include <fstream>
#include <iostream>
#include <sstream>

static void handleMessagebusRequest(messagebus::Message msg)
{
    try
    {
        std::cout << "#########################################################" << std::endl;
        std::cout << "Message received: " << std::endl;
        for (const auto &item : msg.metaData())
        {
            std::cout << "- metaData - " << item.first.c_str() << " : " << item.second.c_str() << std::endl;
        }
        for (const auto &item : msg.userData())
        {
            std::cout << item << std::endl;
        }
        std::cout << "#########################################################" << std::endl;
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

volatile bool _continue = true;

void handler(int s)
{
    std::cerr << "Received signal " << s << std::endl;
    _continue = false;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "./asset-test CREATE      <FILE>" << std::endl;
        std::cout << "./asset-test UPDATE      <FILE>" << std::endl;
        std::cout << "./asset-test DELETE      <INAME> [...]" << std::endl;
        std::cout << "./asset-test GET         <INAME>" << std::endl;
        std::cout << "./asset-test GET_BY_UUID <UUID>" << std::endl;
        std::cout << "./asset-test LIST        [FILE]" << std::endl;
        exit(1);
    }

    std::string op = argv[1];

    signal(SIGINT, handler);
    const char *endpoint = "ipc://@/malamute";

    std::unique_ptr<messagebus::MessageBus> interface(messagebus::MlmMessageBus(endpoint, "test-discovery"));
    messagebus::Message msg;

    interface->connect();
    interface->receive("test-queue", &handleMessagebusRequest);

    msg.metaData().emplace(messagebus::Message::CORRELATION_ID, messagebus::generateUuid());
    msg.metaData().emplace(messagebus::Message::SUBJECT, op);
    msg.metaData().emplace(messagebus::Message::FROM, "test-discovery");
    msg.metaData().emplace(messagebus::Message::TO, "asset-agent-ng");
    msg.metaData().emplace(messagebus::Message::REPLY_TO, "test-queue");

    if (op == "CREATE")
    {
        if (argc < 3)
        {
            std::cout << "./asset-test CREATE <FILE>" << std::endl;
            exit(1);
        }

        std::string assetJson;
        std::ifstream file;
        file.open(argv[2]);

        std::ostringstream stream;
        stream << file.rdbuf();
        assetJson = stream.str();

        msg.userData().push_back(assetJson);

        file.close();
    }
    else if (op == "UPDATE")
    {
        if (argc < 3)
        {
            std::cout << "./asset-test UPDATE <FILE>" << std::endl;
            exit(1);
        }

        std::string assetJson;

        std::ifstream file;
        file.open(argv[2]);

        std::ostringstream stream;
        stream << file.rdbuf();
        assetJson = stream.str();

        msg.userData().push_back(assetJson);

        file.close();
    }
    else if (op == "DELETE")
    {
        if (argc < 2)
        {
            std::cout << "./asset-test DELETE [recursive] <INAME> [...]" << std::endl;
            exit(1);
        }

        std::vector<std::string> inames;

        int c = 2;
        while (c < argc)
        {
            if (std::string(argv[c]) == "recursive")
            {
                msg.metaData().emplace("RECURSIVE", "YES");
            }

            inames.push_back(argv[c++]);
        }

        cxxtools::SerializationInfo list;
        list <<= inames;

        std::stringstream output;
        cxxtools::JsonSerializer serializer(output);
        serializer.serialize(list);

        msg.userData().push_back(output.str());
    }
    else if (op == "GET")
    {
        if (argc < 3)
        {
            std::cout << "./asset-test GET <INAME>" << std::endl;
            exit(1);
        }
        std::string iname = argv[2];

        msg.userData().push_back(iname);
    }
    else if (op == "GET_BY_UUID")
    {
        if (argc < 3)
        {
            std::cout << "./asset-test GET_BY_UUID <UUID>" << std::endl;
            exit(1);
        }
        std::string iname = argv[2];

        msg.userData().push_back(iname);
    }
    else if (op == "LIST")
    {
        if (argc > 2)
        {
            std::string filterJson;

            std::ifstream file;
            file.open(argv[2]);

            std::ostringstream stream;
            stream << file.rdbuf();
            filterJson = stream.str();

            msg.userData().push_back(filterJson);

            file.close();
        }
    }
    else
    {
        std::cout << "Invalid operation" << std::endl;
        exit(2);
    }

    std::cout << "#########################################################" << std::endl;
    std::cout << "Message sent: " << std::endl;
    for (const auto &item : msg.metaData())
    {
        std::cout << "- metaData - " << item.first.c_str() << " : " << item.second.c_str() << std::endl;
    }
    for (const auto &item : msg.userData())
    {
        std::cout << "- userData" << std::endl;
        std::cout << item << std::endl;
    }
    std::cout << "#########################################################" << std::endl;

    interface->sendRequest("FTY.Q.ASSET.QUERY", msg);

    sleep(2);
    return 0;
}
