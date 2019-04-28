// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <dlfcn.h>

#include "endpoint_error.h"
#include "endpoint.h"

namespace horace {

endpoint::endpoint(const std::string& name):
	_name(name) {}

std::string endpoint::suffix() const {
	size_t index = _name.find(':');
	if (index == std::string::npos) {
		throw endpoint_error("missing endpoint prefix in " + name());
	}
	return _name.substr(index + 1, std::string::npos);
}

std::unique_ptr<endpoint> endpoint::make(const std::string& name) {
	size_t index = name.find(':');
	if (index == std::string::npos) {
		throw endpoint_error("missing prefix in endpoint " + name);
	}
	std::string prefix = name.substr(0, index);

	std::string pathname = std::string(
		LIBEXECDIR "/" PKGNAME "/endpoints/") + prefix + ".so";
	void* dlh = dlopen(pathname.c_str(), RTLD_NOW);
	if (!dlh) {
		throw endpoint_error(
			std::string("failed to load endpoint driver: ") +
			dlerror());
	}

	typedef std::unique_ptr<endpoint> endpoint_maker(std::string name);
	endpoint_maker* maker = reinterpret_cast<endpoint_maker*>(
		dlsym(dlh, "make_endpoint"));
	if (!maker) {
		throw endpoint_error(
			"make_endpoint function missing from driver");
	}
	return (*maker)(name);
}

} /* namespace horace */
