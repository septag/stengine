#include "log_format_proxy.h"

#include <stdio.h>
#include <stdarg.h>

#include "bxx/json.h"

static bx::DefaultAllocator gAlloc;

void tee::LogFormatProxy::fatal(const char* fmt, ...)
{
    char text[4096];

    va_list args;
    va_start(args, fmt);
    vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);

    switch (m_options) {
    case LogProxyOptions::Json:
    {
        bx::JsonNodeAllocator alloc(&gAlloc);
        bx::JsonNode* jroot = bx::createJsonNode(&alloc, nullptr, bx::JsonType::Object);
        bx::JsonNode* jerr = bx::createJsonNode(&alloc, "fatal")->setString(text);
        jroot->addChild(jerr);
        char* jsonText = bx::makeJson(jroot, &gAlloc, true);
        jroot->destroy();
        if (jsonText) {
            puts(jsonText);
            BX_FREE(&gAlloc, jsonText);
        }
        break;
    }

    case LogProxyOptions::Text:
    {
        puts(text);
        break;
    }
    }
}

void tee::LogFormatProxy::warn(const char* fmt, ...)
{
    char text[4096];

    va_list args;
    va_start(args, fmt);
    vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);

    switch (m_options) {
    case LogProxyOptions::Json:
    {
        bx::JsonNodeAllocator alloc(&gAlloc);
        bx::JsonNode* jroot = bx::createJsonNode(&alloc);
        bx::JsonNode* jerr = bx::createJsonNode(&alloc, "warning")->setString(text);
        jroot->addChild(jerr);
        char* jsonText = bx::makeJson(jroot, &gAlloc, true);
        jroot->destroy();
        if (jsonText) {
            puts(jsonText);
            BX_FREE(&gAlloc, jsonText);
        }
        break;
    }

    case LogProxyOptions::Text:
    {
        puts(text);
        break;
    }
    }
}

void tee::LogFormatProxy::text(const char* fmt, ...)
{
    char text[4096];

    va_list args;
    va_start(args, fmt);
    vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);

    switch (m_options) {
    case LogProxyOptions::Json:
    {
        bx::JsonNodeAllocator alloc(&gAlloc);
        bx::JsonNode* jroot = bx::createJsonNode(&alloc);
        bx::JsonNode* jerr = bx::createJsonNode(&alloc, "text")->setString(text);
        jroot->addChild(jerr);
        char* jsonText = bx::makeJson(jroot, &gAlloc, true);
        jroot->destroy();
        if (jsonText) {
            puts(jsonText);
            BX_FREE(&gAlloc, jsonText);
        }
        break;
    }

    case LogProxyOptions::Text:
    {
        puts(text);
        break;
    }
    }
}
