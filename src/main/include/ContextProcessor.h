#pragma once

#include <filesystem>

#include "datatypes/ActionType.h"
#include "datatypes/CliStruct.h"
#include "datatypes/Context.h"
#include "datatypes/Data.h"
#include "datatypes/Profile.h"
#include "datatypes/Rules.h"
#include "parser/FileParser.h"

namespace prebyte {

class ContextProcessor {
 private:
  CliStruct cli_struct;
  Context context;
  ActionType action_type;

  void determine_action_type();
  void load_settings();
  void load_profiles();
  void load_variables();
  void load_ignore();
  void load_rules();
  void load_rules(const std::map<std::string, std::string>& rules);
  void load_action_type();
  void inject_variables(const std::string& filePath);
  std::optional<std::filesystem::path> find_settings_file(
      const std::filesystem::path& path) const;

  std::map<std::string, std::string> get_variables(const Data& variables);
  std::map<std::string, Profile> get_profiles(const Data& profiles);
  std::unordered_set<std::string> get_ignore(const Data& ignore);
  std::map<std::string, std::string> get_rules(const Data& rules);
  void add_rule(Rules& rules, std::string rule_name, const Data& rule_data);

  bool get_bool(Data data) const;
  std::string get_string(Data data) const;
  int get_int(Data data) const;
  double get_double(Data data) const;

  void merge_rules(Rules& target,
                   const std::map<std::string, std::string>& source);

 public:
  ContextProcessor(const CliStruct& cli_struct);
  Context process();
};

}  // namespace prebyte
