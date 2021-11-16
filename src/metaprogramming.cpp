#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <rttr/type>
#include <rttr/registration>

#include "metaprogramming.hpp"

#include "state.hpp"

using json = nlohmann::json;

using tb::Foo;
using tb::ByteSlice;
using tb::FileManager;

RTTR_REGISTRATION
{
    using namespace rttr;
    registration::class_<Foo>("Foo")
                 .property("bar", &Foo::bar)
    ;
    registration::class_<ByteSlice>("ByteSlice")
                 .property("start", &ByteSlice::start)
                 .property("end", &ByteSlice::end)
    ;
    registration::class_<FileManager>("FileManager")
                 .property("file_name", &FileManager::file_name)
                 .property("contents", &FileManager::contents)
                 .property("line_start_byte_indices", &FileManager::line_start_byte_indices)
                 .property("match_byte_slices", &FileManager::match_byte_slices)
    ;
/*     registration::class_<TensorId>("TensorId") */
/*                  .property("id", &TensorId::id) */
/*     ; */
/*     registration::class_<TensorDataId>("TensorDataId") */
/*                  .property("id", &TensorDataId::id) */
/*     ; */
/*     registration::class_<OpId>("OpId") */
/*                  .property("id", &OpId::id) */
/*     ; */
/*     registration::class_<Tensor>("Tensor") */
/*                  .property("id", &Tensor::id) */
/*                  .property("shape", &Tensor::shape) */
/*                  .property("shape_ptr", &Tensor::shape)(policy::prop::bind_as_ptr) */
/*     ; */
/*     registration::class_<BufferSize>("BufferSize") */
/*                  .property("count", &BufferSize::count) */
/*     ; */
/*     registration::class_<TensorData>("TensorData") */
/*                  .property("id", &TensorData::id) */
/*                  .property("data", &TensorData::data) */
/*                  .property("size", &TensorData::size) */
/*     ; */
/*     registration::class_<Op>("Op") */
/*                  .property("id", &Op::id) */
/*                  .property("type", &Op::type) */
/*     ; */
/*     registration::class_<Tensor_Op_Association>("Tensor_Op_Association") */
/*                  .property("tensor_id", &Tensor_Op_Association::tensor_id) */
/*                  .property("op_id", &Tensor_Op_Association::op_id) */
/*                  .property("tensor_type", &Tensor_Op_Association::tensor_type) */
/*     ; */
/*     registration::class_<Tensor_TensorData_Association>("Tensor_TensorData_Association") */
/*                  .property("tensor_id", &Tensor_TensorData_Association::tensor_id) */
/*                  .property("tensor_data_id", &Tensor_TensorData_Association::tensor_data_id) */
/*     ; */
/*     registration::class_<EdgeId>("EdgeId") */
/*                  .property("id", &EdgeId::id) */
/*     ; */
/*     registration::class_<NodeId>("NodeId") */
/*                  .property("id", &NodeId::id) */
/*     ; */
/*     registration::class_<GraphId>("GraphId") */
/*                  .property("id", &Graph::id) */
/*     ; */
/*     registration::class_<Node>("Node") */
/*                  .property("id", &Node::id) */
/*                  .property("graph_id", &Node::graph_id) */
/*                  .property("name", &Node::name) */
/*                  .property("type", &Node::type) */
/*     ; */
/*     registration::class_<Edge>("Edge") */
/*                  .property("id", &Edge::id) */
/*                  .property("src", &Edge::src) */
/*                  .property("sink", &Edge::sink) */
/*     ; */
/*     registration::class_<Graph>("Graph") */
/*                  .property("id", &Graph::id) */
/*     ; */
/*  */
/*     registration::class_<Node_Edge_Association>("Node_Edge_Association") */
/*                  .property("node_id", &Node_Edge_Association::node_id) */
/*                  .property("edge_id", &Node_Edge_Association::edge_id) */
/*     ; */
/*     registration::class_<Node_OperandEdge_Association>("Node_OperandEdge_Association") */
/*                  .property("id", &Node_OperandEdge_Association::id) */
/*                  .property("sink", &Node_OperandEdge_Association::sink) */
/*     ; */
/*     registration::class_<Node_UserEdge_Association>("Node_UserEdge_Association") */
/*                  .property("id", &Node_UserEdge_Association::id) */
/*                  .property("src", &Node_UserEdge_Association::src) */
/*     ; */
}


namespace tb::meta
{

using namespace rttr;

bool is_primitive_type(const type& obj_type)
{
    return
        obj_type.is_arithmetic()
        or (obj_type == type::get<std::string>())
        or (obj_type == type::get<String>())
        or (obj_type == type::get<StringRef>())
        or obj_type.is_enumeration();
}

json to_json_primitive_type(const variant& object)
{
    json result;

    type obj_type = object.get_type();

    if (obj_type.is_arithmetic())
    {
        if (obj_type == type::get<bool>())
            result = object.to_bool();
        else if (obj_type == type::get<int>())
            result = object.to_int();
        else if (obj_type == type::get<uint8_t>())
            result = object.to_uint8();
        else if (obj_type == type::get<uint16_t>())
            result = object.to_uint16();
        else if (obj_type == type::get<uint32_t>())
            result = object.to_uint32();
        else if (obj_type == type::get<uint64_t>())
            result = object.to_uint64();
        else if (obj_type == type::get<int8_t>())
            result = object.to_int8();
        else if (obj_type == type::get<int16_t>())
            result = object.to_int16();
        else if (obj_type == type::get<int32_t>())
            result = object.to_int32();
        else if (obj_type == type::get<int64_t>())
            result = object.to_int64();
        else
            throw;
    }
    else if (obj_type == type::get<String>())
    {
        result = object.to_string();
    }
    else if (obj_type == type::get<StringRef>())
    {
        result = object.to_string();
    }
    return result;
}

json to_json_aggregate_type(instance object)
{
    json result;

    type obj_type = object.get_type();

    std::string prop_name_and_values;
    for (property prop : obj_type.get_properties())
    {
        string_view prop_name = prop.get_name();
        variant prop_value = prop.get_value(object);
        if (prop_value.is_valid())
        {
            result[prop_name.to_string()] = to_json_variant(prop_value);
        }
        else
        {
            result[prop_name.to_string()] = "unregistered_type!";
        }
    }
    return result;
}

json to_json_variant(const variant& object)
{
    json result;

    if (not object.is_valid())
    {
        return result;
    }

    type obj_type = object.get_type();

    if (is_primitive_type(obj_type))
    {
        result = to_json_primitive_type(object);
    }
    else if (is_primitive_type(obj_type.get_wrapped_type()))
    {
        result = to_json_primitive_type(object.extract_wrapped_value());
    }
    else
    {
        result = to_json_aggregate_type(object);
    }

    return result;
}

json to_json(const variant& object)
{
    json result;

    if (not object.is_valid())
    {
        return result;
    }
    else if (object.is_sequential_container())
    {
        for (const auto& item: object.create_sequential_view())
        {
            result.push_back(to_json(item));
        }
        return result;
    }
    else
    {
        return to_json_variant(object);
    }
}

std::string to_string(const variant& object)
{
    std::string result;

    if (not object.is_valid())
    {
        return "unregistered_type!";
    }
    U32 indent = 4;
    std::string guts_as_string = to_json(object).dump(indent);

    type obj_type = object.get_type();
    return fmt::format("{} {}", obj_type.get_name().to_string(), guts_as_string);
}

bool generic_eq_operator(const variant& a, const variant& b)
{
    // A simple comparator, leveraging the json implementation.
    rttr::type obj_type = a.get_type();
    if (not obj_type.is_valid())
    {
        throw;
    }
    return tb::meta::to_json(a) == tb::meta::to_json(b);
}

bool generic_neq_operator(const variant& a, const variant& b)
{
    return not generic_eq_operator(a, b);
}

}
