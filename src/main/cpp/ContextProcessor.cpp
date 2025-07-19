#include "ContextProcessor.h"

namespace prebyte {

ContextProcessor::ContextProcessor(const CliStruct& cli_struct)
    : cli_struct(cli_struct) {
}

Context ContextProcessor::process() {
    load_settings();
    load_profiles();
    load_variables();
    load_ignore();
    load_rules();
    return this->context;
}

void ContextProcessor::load_settings() {
}

void ContextProcessor::load_profiles() {
}

void ContextProcessor::load_variables() {
}

void ContextProcessor::load_ignore() {
}

void ContextProcessor::load_rules() {
}

}
