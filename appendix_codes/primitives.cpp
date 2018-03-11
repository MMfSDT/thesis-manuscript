/* Copyright 2013-present Barefoot Networks, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Antonin Bas (antonin@barefootnetworks.com)
 *
 */

#include <bm/bm_sim/actions.h>
#include <bm/bm_sim/core/primitives.h>
#include <cstdlib>
#include <ctime>

template <typename... Args>
using ActionPrimitive = bm::ActionPrimitive<Args...>;

using bm::Data;
using bm::Field;
using bm::Header;

class modify_field : public ActionPrimitive<Field &, const Data &> {
  void operator ()(Field &f, const Data &d) {
    bm::core::assign()(f, d);
  }
};

REGISTER_PRIMITIVE(modify_field);

class add_to_field : public ActionPrimitive<Field &, const Data &> {
  void operator ()(Field &f, const Data &d) {
    f.add(f, d);
  }
};

REGISTER_PRIMITIVE(add_to_field);

class drop : public ActionPrimitive<> {
  void operator ()() {
    get_field("standard_metadata.egress_spec").set(511);
  }
};

REGISTER_PRIMITIVE(drop);

class modify_field_rng_uniform : public ActionPrimitive<Field &, const Data &, const Data &> {
	unsigned int g_seed = -1;
	bool seeded = false;

	// Used to seed the generator.
	inline void fast_srand(int seed) {
	    g_seed = seed;
	    seeded = true;
	}

	// Compute a pseudorandom integer.
	// Output value in range [0, 32767]
	inline int fast_rand(int bits) {
	    g_seed = (214013*g_seed+2531011);
	    return (g_seed>>16)&bits;
	}


	void operator()(Field &f, const Data &a, const Data &b) {
		if(!seeded)
			fast_srand(time(0));
		Data d(fast_rand(b.get_int()));
		bm::core::assign()(f,d);
	}
};

REGISTER_PRIMITIVE(modify_field_rng_uniform);