//
//  binding.cc
//  Easylogging++ Node.js Binding
//
//  Copyright (C) 2017-present Zuhd Web Services
//  Copyright (C) 2017-present @abumusamq
//
//  https://zuhd.org
//  https://muflihun.com
//  https://github.com/zuhd-org/easyloggingpp-node
//
//  LICENSE @ https://github.com/zuhd-org/easyloggingpp-node/blob/master/LICENSE
//

#include <nan.h>
#include <easylogging++.h>

using v8::FunctionTemplate;
using v8::Isolate;
using v8::String;
using Nan::To;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

INITIALIZE_EASYLOGGINGPP

NAN_METHOD(Version)
{
    info.GetReturnValue().Set(String::NewFromUtf8(Isolate::GetCurrent(), el::VersionInfo::version().c_str()));
}

NAN_METHOD(AddFlag)
{
    if (info[0]->IsNumber()) {
        el::LoggingFlag loggingFlag = static_cast<el::LoggingFlag>(To<el::base::type::EnumType>(info[0]).FromJust());
        el::Loggers::addFlag(loggingFlag);
    } else {
        return Nan::ThrowError(Nan::Error("Invalid LoggingFlag. Please use easloggingpp.LoggingFlag"));
    }
}

NAN_METHOD(RemoveFlag)
{
    if (info[0]->IsNumber()) {
        el::LoggingFlag loggingFlag = static_cast<el::LoggingFlag>(To<el::base::type::EnumType>(info[0]).FromJust());
        el::Loggers::removeFlag(loggingFlag);
    } else {
        return Nan::ThrowError(Nan::Error("Invalid LoggingFlag. Please use easloggingpp.LoggingFlag"));
    }
}

NAN_METHOD(HasFlag)
{
    if (info[0]->IsNumber()) {
        el::LoggingFlag loggingFlag = static_cast<el::LoggingFlag>(To<el::base::type::EnumType>(info[0]).FromJust());
        bool has = el::Loggers::hasFlag(loggingFlag);
        info.GetReturnValue().Set(has ? Nan::True() : Nan::False());
    } else {
        return Nan::ThrowError(Nan::Error("Invalid LoggingFlag. Please use easloggingpp.LoggingFlag"));
    }
}

NAN_METHOD(RegisterLogger)
{
    if (!info[0]->IsString()) {
        return Nan::ThrowError(Nan::Error("Invalid logger ID. Please provide string."));
    }
    const std::string loggerId = *Nan::Utf8String(info[0]);

    if (!el::Logger::isValidId(loggerId)) {
        return Nan::ThrowError(Nan::Error("Invalid logger ID. Allowed characters are alpha-numeric, hyphen, underscore or dot"));
    }
    el::Loggers::getLogger(loggerId, true);
}

NAN_METHOD(ConfigureFromFile)
{
    if (!info[0]->IsString()) {
        return Nan::ThrowError(Nan::Error("Invalid logger ID. Please provide string."));
    }
    const std::string loggerId = *Nan::Utf8String(info[0]);

    if (!info[1]->IsString()) {
        return Nan::ThrowError(Nan::Error("Invalid config file. Please provide string."));
    }
    const std::string file = *Nan::Utf8String(info[1]);

    el::Configurations conf(file);
    el::Logger* logger = el::Loggers::getLogger(loggerId, true);
    el::Loggers::reconfigureLogger(logger, conf);
}

NAN_METHOD(ConfigureValue)
{
    if (!info[0]->IsString()) {
        return Nan::ThrowError(Nan::Error("Invalid logger ID. Please provide string."));
    }
    const std::string loggerId = *Nan::Utf8String(info[0]);

    el::ConfigurationType config = el::ConfigurationType::Unknown;
    if (info[1]->IsNumber()) {
        config = static_cast<el::ConfigurationType>(To<el::base::type::EnumType>(info[1]).FromJust());
    } else {
        return Nan::ThrowError(Nan::Error("Invalid ConfigType. Please use easyloggingpp.ConfigType"));
    }

    if (!info[2]->IsString()) {
        return Nan::ThrowError(Nan::Error("Invalid config value. Please provide string."));
    }
    const std::string value = *Nan::Utf8String(info[2]);

    el::Logger* logger = el::Loggers::getLogger(loggerId, true);
    el::Loggers::reconfigureLogger(logger->id(), config, value);
}

NAN_METHOD(ConfigureAllFromFile)
{
    if (!info[0]->IsString()) {
        return Nan::ThrowError(Nan::Error("Invalid config file. Please provide string."));
    }
    const std::string file = *Nan::Utf8String(info[0]);

    el::Configurations conf(file);
    el::Loggers::setDefaultConfigurations(conf, true);
}

NAN_METHOD(ConfigureAllValue)
{
    el::ConfigurationType config = el::ConfigurationType::Unknown;
    if (info[0]->IsNumber()) {
        config = static_cast<el::ConfigurationType>(To<el::base::type::EnumType>(info[0]).FromJust());
    } else {
        return Nan::ThrowError(Nan::Error("Invalid ConfigType. Please use easyloggingpp.ConfigType"));
    }

    if (!info[1]->IsString()) {
        return Nan::ThrowError(Nan::Error("Invalid config value. Please provide string."));
    }
    const std::string value = *Nan::Utf8String(info[1]);

    el::Loggers::reconfigureAllLoggers(config, value);
}

NAN_METHOD(ConfigureAllValueByLevel)
{
    el::Level level = el::Level::Unknown;
    if (info[0]->IsNumber()) {
        level = static_cast<el::Level>(To<el::base::type::EnumType>(info[0]).FromJust());
    } else {
        return Nan::ThrowError(Nan::Error("Invalid Level. Please use easyloggingpp.Level"));
    }

    el::ConfigurationType config = el::ConfigurationType::Unknown;
    if (info[1]->IsNumber()) {
        config = static_cast<el::ConfigurationType>(To<el::base::type::EnumType>(info[1]).FromJust());
    } else {
        return Nan::ThrowError(Nan::Error("Invalid ConfigType. Please use easyloggingpp.ConfigType"));
    }

    if (!info[2]->IsString()) {
        return Nan::ThrowError(Nan::Error("Invalid config value. Please provide string."));
    }
    const std::string value = *Nan::Utf8String(info[2]);

    el::Loggers::reconfigureAllLoggers(level, config, value);
}

NAN_METHOD(WriteLog) {
    const std::string loggerId = *Nan::Utf8String(info[0]);
    const std::string file = *Nan::Utf8String(info[1]);
    const std::string func = *Nan::Utf8String(info[3]);
    const std::string msg = *Nan::Utf8String(info[4]);
    el::Level level = el::Level::Info;
    el::base::type::LineNumber line = 0;
    el::base::type::VerboseLevel vl = 0;

    if (info[5]->IsNumber()) {
        level = static_cast<el::Level>(To<el::base::type::EnumType>(info[5]).FromJust());
    }
    if (info[2]->IsNumber() && info[2]->IsNumber()) {
        line = static_cast<el::base::type::LineNumber>(To<el::base::type::EnumType>(info[2]).FromJust());
    }
    if (info[6]->IsNumber()) {
        vl = static_cast<el::base::type::VerboseLevel>(To<el::base::type::EnumType>(info[6]).FromJust());
    }

    el::base::Writer(level,
                     file.c_str(),
                     line, func.c_str(),
                     el::base::DispatchAction::NormalLog, vl)
    .construct(1, loggerId.c_str()) << msg;
}

NAN_MODULE_INIT(InitAll)
{
    el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::AllowVerboseIfModuleNotSpecified);
    el::Loggers::addFlag(el::LoggingFlag::IgnoreSigInt);

    #define DEFINE_FN(name, callback) Set(target, New<String>(#name).ToLocalChecked(), \
        GetFunction(New<FunctionTemplate>(callback)).ToLocalChecked())

    DEFINE_FN(version, Version);
    DEFINE_FN(configure_from_file, ConfigureFromFile);
    DEFINE_FN(configure, ConfigureValue);
    DEFINE_FN(configure_all_loggers_from_file, ConfigureAllFromFile);
    DEFINE_FN(configure_all_loggers, ConfigureAllValue);
    DEFINE_FN(configure_all_loggers_by_level, ConfigureAllValueByLevel);
    DEFINE_FN(write_log, WriteLog);
    DEFINE_FN(register_logger, RegisterLogger);
    DEFINE_FN(add_flag, AddFlag);
    DEFINE_FN(remove_flag, RemoveFlag);
    DEFINE_FN(has_flag, HasFlag);
    #undef DEFINE_FN
}


NODE_MODULE(easyloggingpp, InitAll)
