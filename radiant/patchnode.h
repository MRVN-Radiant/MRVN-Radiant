/*
   Copyright (C) 2001-2006, William Joseph.
   All Rights Reserved.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "instancelib.h"
#include "patch.h"

#include "map/mapwriter.h"

template<typename TokenImporter, typename TokenExporter>
class PatchNode :
	public scene::Node::Symbiot,
	public scene::Instantiable,
	public scene::Cloneable
{
	typedef PatchNode<TokenImporter, TokenExporter> Self;

	class TypeCasts
	{
		InstanceTypeCastTable m_casts;
	public:
		TypeCasts() {
			NodeStaticCast<PatchNode, scene::Instantiable>::install(m_casts);
			NodeStaticCast<PatchNode, scene::Cloneable>::install(m_casts);
			NodeContainedCast<PatchNode, Snappable>::install(m_casts);
			NodeContainedCast<PatchNode, TransformNode>::install(m_casts);
			NodeContainedCast<PatchNode, Patch>::install(m_casts);
			NodeContainedCast<PatchNode, XMLImporter>::install(m_casts);
			NodeContainedCast<PatchNode, XMLExporter>::install(m_casts);
			NodeContainedCast<PatchNode, MapImporter>::install(m_casts);
			NodeContainedCast<PatchNode, MapExporter>::install(m_casts);
			NodeContainedCast<PatchNode, Nameable>::install(m_casts);
			NodeContainedCast<PatchNode, IPatchExporter>::install(m_casts);
		}
		InstanceTypeCastTable& get() {
			return m_casts;
		}
	};


	scene::Node m_node;
	InstanceSet m_instances;
	Patch m_patch;
	TokenImporter m_importMap;
	TokenExporter m_exportMap;

	CPatchExporter m_PatchExporter;

public:

	typedef LazyStatic<TypeCasts> StaticTypeCasts;

	Snappable& get(NullType<Snappable>) {
		return m_patch;
	}
	TransformNode& get(NullType<TransformNode>) {
		return m_patch;
	}
	Patch& get(NullType<Patch>) {
		return m_patch;
	}
	XMLImporter& get(NullType<XMLImporter>) {
		return m_patch;
	}
	XMLExporter& get(NullType<XMLExporter>) {
		return m_patch;
	}
	MapImporter& get(NullType<MapImporter>) {
		return m_importMap;
	}
	MapExporter& get(NullType<MapExporter>) {
		return m_exportMap;
	}
	Nameable& get(NullType<Nameable>) {
		return m_patch;
	}
	IPatchExporter& get(NullType<IPatchExporter>)
	{
		return m_PatchExporter;
	}

	PatchNode(bool patchDef3 = false) :
		m_node(this, this, StaticTypeCasts::instance().get()),
		m_patch(m_node, InstanceSetEvaluateTransform<PatchInstance>::Caller(m_instances), InstanceSet::BoundsChangedCaller(m_instances)),
		m_importMap(m_patch),
		m_exportMap(m_patch),
		m_PatchExporter(m_patch){
		m_patch.m_patchDef3 = patchDef3;
	}
	PatchNode(const PatchNode& other) :
		scene::Node::Symbiot(other),
		scene::Instantiable(other),
		scene::Cloneable(other),
		m_node(this, this, StaticTypeCasts::instance().get()),
		m_patch(other.m_patch, m_node, InstanceSetEvaluateTransform<PatchInstance>::Caller(m_instances), InstanceSet::BoundsChangedCaller(m_instances)),
		m_importMap(m_patch),
		m_exportMap(m_patch),
		m_PatchExporter(m_patch){
	}
	void release() {
		delete this;
	}
	scene::Node& node() {
		return m_node;
	}
	Patch& get() {
		return m_patch;
	}
	const Patch& get() const {
		return m_patch;
	}

	scene::Node& clone() const {
		return (new PatchNode(*this))->node();
	}

	scene::Instance* create(const scene::Path& path, scene::Instance* parent) {
		return new PatchInstance(path, parent, m_patch);
	}
	void forEachInstance(const scene::Instantiable::Visitor& visitor) {
		m_instances.forEachInstance(visitor);
	}
	void insert(scene::Instantiable::Observer* observer, const scene::Path& path, scene::Instance* instance) {
		m_instances.insert(observer, path, instance);
	}
	scene::Instance* erase(scene::Instantiable::Observer* observer, const scene::Path& path) {
		return m_instances.erase(observer, path);
	}
};

typedef PatchNode<PatchTokenImporter, PatchTokenExporter> PatchNodeQuake3;
typedef PatchNode<PatchDoom3TokenImporter, PatchDoom3TokenExporter> PatchNodeDoom3;

inline Patch* Node_getPatch(scene::Node& node) {
	return NodeTypeCast<Patch>::cast(node);
}