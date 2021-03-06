/*
 * This source is part of the single graph mining algorithm.
 *
 * Copyright 2014 Robert Kessl
 * Copyright 2014-2016 Nilothpal Talukder
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <logger.hpp>
#include <graph_output.hpp>
#include <graph_types.hpp>
#include <utils.hpp>
#include <dbio.hpp>
#include <graph_miner.hpp>
#include <sys/time.h>

using std::cout;
using std::endl;
using std::cerr;
using types::Graph;

Logger *value_logger = Logger::get_logger("VAL");
Logger *logger = Logger::get_logger("MAIN");

int main(int argc, char **argv)
{
  if(argc != 5 && argc != 4) {
    std::cerr << "usage: \n" << argv[0] << " <filetype> <filename> <support-int-abs> [<output-filename>]" << std::endl;
    return 1;
  }


  string filetype = argv[1];
  string filename = argv[2];
  int absolute_min_support = atoi(argv[3]);
  string output_filename;
  if(argc == 5) output_filename = argv[4];

  Graph graph;

  dbio::FILE_TYPE file_format;
  file_format = dbio::ftype2str(string(argv[1]));

  dbio::read_graph(file_format, filename, graph);

  if(absolute_min_support < 1) {
    cerr << "error: absolute_min_support < 1" << endl;
    return 3;
  }

  graph_counting_output *count_out = new graph_counting_output();
  graph_file_output *file_out = 0;

  graph_output *current_out = count_out;

  if(output_filename.size() > 0 && output_filename != "-") {
    file_out = new graph_file_output(output_filename);
    current_out = file_out;
  }


  timeval start_time, stop_time;
  gettimeofday(&start_time, 0);

  LOG_VAL(*value_logger, absolute_min_support);
  LOG_VALUE(*value_logger, "output", current_out->to_string());

  GRAPH_MINER::graph_miner fsm;

  fsm.set_graph(graph);
  fsm.set_min_support(absolute_min_support);
  fsm.set_graph_output(current_out);
  fsm.run();

  gettimeofday(&stop_time, 0);

  LOG_VALUE(*value_logger, "total_time", std::fixed << utils::get_time_diff(start_time, stop_time));
  LOG_VALUE(*value_logger, "total_frequent_graphs", (int) current_out->get_count());

  INFO(*logger, "clean-up...");

  delete count_out;
  delete file_out;

  return 0;
}

