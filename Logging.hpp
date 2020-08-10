#pragma once

#include <map>
#include <set>
#include <tuple>
#include <chrono>
#include <fstream>
#include <optional>
#include <shared_mutex>
#include <cassert>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/format.hpp>

#include "FileSystem.hpp"
#include "Private.h"

using Uuid = boost::uuids::uuid;

/**
*   class ILoggable
*
*   @brief Интерфейс параметризованного лога вызова метода формате [BEGIN-END], позволяющего выводить
*   разнообразную информацию о методах объекта, реализующего интерфйс ILoggable, включая тайминги выполнения метода.
*
*   Поддерживает вложенные блоки
*
*   Может работать в многопоточной среде 
*
*   Шаблонный параметр - таймер, например Qt Timer либо стандартный таймер STL
*/
template<typename T = LogTimer>
class ILoggable
{
	static const int TIMER_POS = 3;

public:
	virtual std::string logInfo() const = 0;

public:
	//NOTE: success, id, output_string
	using LogResultStart = std::tuple<Uuid, std::string>;

	//NOTE: success,  output_string, timing
	using LogResultEnd = std::tuple<bool, std::string, std::optional<int> >;	

	LogResultStart logStartBase(const std::string& name, std::optional<std::string> caption = std::nullopt,
		std::optional<std::string> params = std::nullopt, std::optional<std::string> from = std::nullopt, 
		bool logDebugMode = false, bool save = false, bool useTiming = true	)
	{
		auto finalCaption{ caption ? *caption : std::string() };
		auto finalParams{ params ? *params : std::string() };
		auto finalFrom{ from ? *from : "???" };			

		LogData logData{name, finalCaption, logDebugMode, std::nullopt };

		std::stringstream ss;
		ss << ">>>>>>>> _TR_:" << System::threadName() << "(" << std::this_thread::get_id() << ")| " << finalCaption << "| " << logInfo() <<  finalParams <<  "[from " << finalFrom << "]";

		if (logDebugMode)
		{
			LOG_DEBUG_X(name, "%s", ss.str());
		}
		else
		{
			LOG_INFO_X(name, "%s", ss.str());
		}
		
		if (save)
		{
			std::unique_lock lock(logDataMutex_);

			if (useTiming)
			{
				Timer timer;
				timer.start();
				std::get<TIMER_POS>(logData) = std::move(timer);
			}

			Uuid id = boost::uuids::random_generator()();

			logThreadMap_.insert(std::pair<Key, LogData>(id, logData));

			return LogResultStart{ id, ss.str() };
		}	

		return LogResultStart{Uuid(), ss.str() };
	}

	LogResultStart logStart(const std::string& name, std::optional<std::string> caption = std::nullopt,
		std::optional<std::string> params = std::nullopt, std::optional<std::string> from = std::nullopt)
	{
		return logStartBase(name, caption, params, from, false, false, false);
	}

	LogResultStart logStartD(const std::string& name, std::optional<std::string> caption = std::nullopt,
		std::optional<std::string> params = std::nullopt, std::optional<std::string> from = std::nullopt)
	{
		return logStartBase(name, caption, params, from, true, false, false);
	}

	LogResultStart logStartS(const std::string& name, std::optional<std::string> caption = std::nullopt,
		std::optional<std::string> params = std::nullopt, std::optional<std::string> from = std::nullopt)
	{
		return logStartBase(name, caption, params, from, false, true, true);
	}

	LogResultStart logStartSD(const std::string& name, std::optional<std::string> caption = std::nullopt,
		std::optional<std::string> params = std::nullopt, std::optional<std::string> from = std::nullopt)
	{
		return logStartBase(name, caption, params, from, true, true, true);
	}

	LogResultEnd logEnd(const Uuid & id = Uuid(), std::optional<int> logResult = std::nullopt, std::optional<std::string> params = std::nullopt)
	{
		LogData logData;
		std::optional<int> elapsedTimeMs;

		if(id.is_nil())
			return LogResultEnd{ false, std::string(), std::nullopt };

		{
			std::unique_lock lock(logDataMutex_);

			if (auto it = logThreadMap_.find(id); it != logThreadMap_.end())
			{
				logData = std::move(it->second);
				if (std::get<TIMER_POS>(logData))
					elapsedTimeMs = std::get<TIMER_POS>(logData)->elapsed();
				logThreadMap_.erase(it);
			}	
			else
			{
				return LogResultEnd{ false, std::string(), std::nullopt };
			}
		}
			
		std::string name, caption;
		bool logDebugMode;
		std::tie(name, caption, logDebugMode, std::ignore) = logData;	

		auto finalName = "~" + name;
		auto finalParams{ params ? *params : std::string() };

		std::stringstream ss;
		ss << "<<<<<<<< _TR_:" << System::threadName() << "(" << std::this_thread::get_id() << ")| " << caption << "| " <<  finalParams << " | TIME=__" << (elapsedTimeMs ? std::to_string(*elapsedTimeMs) : "NaN")
			<< "__ | RESULT=__" << (logResult ? std::to_string(*logResult) : "NaN") << "__";
		
		if (logDebugMode)
		{
			LOG_DEBUG_X(finalName, "%s", ss.str());
		}
		else
		{
			LOG_INFO_X(finalName, "%s", ss.str());
		}

		return LogResultEnd{ true, ss.str(), elapsedTimeMs };
	}

private:
	using Timer = T;

	//NOTE: 
	using Key = Uuid;

	//NOTE: name, caption, isDebugLog, timer
	using LogData = std::tuple<std::string, std::string, bool, std::optional<Timer> >;	

	std::shared_mutex            logDataMutex_;
	std::map<Key, LogData>       logThreadMap_;
};


