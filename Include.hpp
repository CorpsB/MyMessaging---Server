#pragma once

#include <string>
#include <sqlite3.h>
#include <exception>
#include <boost/asio.hpp>
#include <memory>
#include <string>

//Models :
#include "DataBase/Models/Role.hpp"
#include "DataBase/Models/Category.hpp"
#include "DataBase/Models/Channel.hpp"
#include "DataBase/Models/Message.hpp"
#include "DataBase/Models/Role.hpp"
#include "DataBase/Models/User.hpp"
#include "DevTools/Error.hpp"

#include "DataBase/Db/Database.hpp"
#include "DataBase/Repository/MessageRepository.hpp"
#include "DataBase/Repository/CategoryRepository.hpp"
#include "DataBase/Repository/ChannelRepository.hpp"
#include "DataBase/Repository/RoleRepository.hpp"
#include "DataBase/Repository/UserRepository.hpp"
#include "Net/Server.hpp"

using tcp = boost::asio::ip::tcp;
