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
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#ifdef HAVE_BOOST_DATE_TIME
	#include <boost/date_time.hpp>
#endif
#include "function.h"
#include "function_info.h"
#include "performance_counter.h"
#include "profile_writer_xml.h"

namespace amx_profiler {

ProfileWriterXml::ProfileWriterXml(std::ostream *stream, const std::string script_name)
	: stream_(stream)
	, script_name_(script_name)
{
}

void ProfileWriterXml::Write(const std::vector<std::shared_ptr<FunctionInfo>> &stats)
{
	*stream_ <<
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
	"<profile script=\"" << script_name_ << "\""
	#ifdef HAVE_BOOST_DATE_TIME
		<< "date=\"" << boost::posix_time::second_clock::local_time() << "\">";
	#else
		<< ">";
	#endif

	TimeInterval time_all = 0;
	std::for_each(stats.begin(), stats.end(), [&](const std::shared_ptr<FunctionInfo> &info) { 
		time_all += info->total_time() - info->child_time(); 
	});

	TimeInterval total_time_all = 0;
	std::for_each(stats.begin(), stats.end(), [&](const std::shared_ptr<FunctionInfo> &info) { 
		total_time_all += info->total_time(); 
	});

	std::for_each(stats.begin(), stats.end(), [&](const std::shared_ptr<FunctionInfo> &info) {
		*stream_ << "		<function";
		*stream_ << " type=\"" << info->function()->type() << "\"";
		*stream_ << " name=\"" << info->function()->name() << "\"";
		*stream_ << " calls=\"" << info->num_calls() << "\"";
		*stream_ << " total_time=\"" <<  std::fixed << std::setprecision(2)
			<< static_cast<double>(info->GetSelfTime() * 100) / time_all << "\"";
		*stream_ << " total_time=\"" <<  std::fixed << std::setprecision(2)
			<< static_cast<double>(info->total_time() * 100) / total_time_all << "\"";
		*stream_ << " />\n";
	});

	*stream_ << "</profile>";
}

} // namespace amx_profiler
