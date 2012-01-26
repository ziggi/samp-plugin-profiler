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

#include <functional>
#include <iostream>
#include <tuple>
#include "call_graph.h"
#include "function.h"
#include "function_info.h"

namespace amx_profiler {

CallGraphNode::CallGraphNode(const std::shared_ptr<FunctionInfo> &info, 
                             const std::shared_ptr<CallGraphNode> &caller) 
	: info_(info)
	, caller_(caller)
	, std::enable_shared_from_this<CallGraphNode>()
{
}

void CallGraphNode::AddCallee(const std::shared_ptr<FunctionInfo> &fn) {
	auto new_node = std::shared_ptr<CallGraphNode>(new CallGraphNode(fn, shared_from_this()));
	AddCallee(new_node);
}

void CallGraphNode::AddCallee(const std::shared_ptr<CallGraphNode> &node) {
	// Avoid duplicate functions
	for (auto iterator = callees_.begin(); iterator != callees_.end(); ++iterator) {
		if ((*iterator)->info()->function()->address() == node->info()->function()->address()) {
			return;
		}
	}
	callees_.push_back(node);
}

void CallGraphNode::Write(std::ostream &stream) const {
	if (!callees_.empty()) {
		std::string caller_name;
		if (info_) {
			caller_name = info_->function()->name();
		} else {
			caller_name = "HOST";
		}
		for (auto iterator = callees_.begin(); iterator != callees_.end(); ++iterator) {			
			std::tuple<double, double, double> color;
			auto type = (*iterator)->info()->function()->type();
			if (!info_ || type == "public") {
				color = std::make_tuple(0.666, 1.000, 1.000);
			} else if (type == "native") {
				color = std::make_tuple(0.002, 1.000, 1.000);
			} else {
				color = std::make_tuple(0.000, 0.000, 0.000);
			}
			stream << '\t' << caller_name << " -> " << (*iterator)->info()->function()->name() 
				<< " [color=\""
					<< std::get<0>(color) << ","
					<< std::get<1>(color) << ","
					<< std::get<2>(color)
				<< "\"];" << std::endl;
			(*iterator)->Write(stream);
		}
	}
}

CallGraph::CallGraph(const std::shared_ptr<CallGraphNode> &root)
	: root_(root)
	, sentinel_(new CallGraphNode(0))
{
	if (!root) {
		root_ = sentinel_;
	}
}

void CallGraph::Write(std::ostream &stream) const {
	stream << 
	"digraph Profile {\n"
	"	size=\"6,4\"; ratio = fill;\n"
	"	node [style=filled];\n"
	;

	// Write all nodes recrusively
	sentinel_->Write(stream);

	stream <<
	"}\n";
}

} // namespace amx_profiler