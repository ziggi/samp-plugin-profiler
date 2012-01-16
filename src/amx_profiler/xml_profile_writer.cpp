// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011-2012 Sergey Zolotarev
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <string>
#include <vector>
#include "function.h"
#include "function_info.h"
#include "performance_counter.h"
#include "xml_profile_writer.h"

namespace amx_profiler {

void XmlProfileWriter::Write(const std::string &script_name, std::ostream &stream,
	const std::vector<FunctionInfoPtr> &stats)
{
	stream <<
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
	"<profile script=\"" << script_name << "\"";

	TimeInterval time_all = 0;
	std::for_each(stats.begin(), stats.end(), [&](const FunctionInfoPtr &info) { 
		time_all += info->total_time() - info->child_time(); 
	});

	TimeInterval total_time_all = 0;
	std::for_each(stats.begin(), stats.end(), [&](const FunctionInfoPtr &info) { 
		total_time_all += info->total_time(); 
	});

	std::for_each(stats.begin(), stats.end(), [&](const FunctionInfoPtr &info) {
		stream << "		<function";
		stream << " type=\"" << info->function()->type() << "\"";
		stream << " name=\"" << info->function()->name() << "\"";
		stream << " calls=\"" << info->num_calls() << "\"";
		stream << " total_time=\"" <<  std::fixed << std::setprecision(2)
			<< static_cast<double>((info->total_time() - info->child_time()) * 100) / time_all << "\"";
		stream << " total_time=\"" <<  std::fixed << std::setprecision(2)
			<< static_cast<double>(info->total_time() * 100) / total_time_all << "\"";
		stream << " />\n";
	});

	stream << "</profile>";
}

} // namespace amx_profiler
