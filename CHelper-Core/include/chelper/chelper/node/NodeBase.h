//
// Created by Yancey on 2023/11/10.
//

#pragma once

#ifndef CHELPER_NODEBASE_H
#define CHELPER_NODEBASE_H

#include "../lexer/TokenReader.h"
#include "../parser/ASTNode.h"
#include "pch.h"

#define CHELPER_NODE_TYPES BLOCK,             \
                           BOOLEAN,           \
                           COMMAND,           \
                           COMMAND_NAME,      \
                           FLOAT,             \
                           INTEGER,           \
                           INTEGER_WITH_UNIT, \
                           ITEM,              \
                           LF,                \
                           NAMESPACE_ID,      \
                           NORMAL_ID,         \
                           PER_COMMAND,       \
                           POSITION,          \
                           RELATIVE_FLOAT,    \
                           REPEAT,            \
                           STRING,            \
                           TARGET_SELECTOR,   \
                           TEXT,              \
                           RANGE,             \
                           JSON,              \
                           JSON_BOOLEAN,      \
                           JSON_ELEMENT,      \
                           JSON_ENTRY,        \
                           JSON_FLOAT,        \
                           JSON_INTEGER,      \
                           JSON_LIST,         \
                           JSON_NULL,         \
                           JSON_OBJECT,       \
                           JSON_STRING,       \
                           AND,               \
                           ANY,               \
                           ENTRY,             \
                           EQUAL_ENTRY,       \
                           LIST,              \
                           OR,                \
                           SINGLE_SYMBOL

// #define CODEC_NODE(CodecType, ...)                                     \
//     CODEC_WITH_PARENT(CodecType, CHelper::Node::NodeBase, __VA_ARGS__) \
//     CODEC_UNIQUE_PTR(CodecType)

#define CODEC_NODE(CodecType, ...) \
    CODEC_WITH_PARENT(CodecType, CHelper::Node::NodeBase, __VA_ARGS__)

#define CODEC_NODE_NONE(CodecType) \
    CODEC_NONE_WITH_PARENT(CodecType, CHelper::Node::NodeBase)

namespace CHelper {

    class CPack;

    namespace Node {

        namespace NodeTypeId {
            enum NodeTypeId : uint8_t {
                CHELPER_NODE_TYPES
            };
        };

        constexpr NodeTypeId::NodeTypeId MAX_TYPE_ID = NodeTypeId::SINGLE_SYMBOL;

        class NodeBase {
        public:
            std::optional<std::u16string> id;
            std::optional<std::u16string> brief;
            std::optional<std::u16string> description;
            //true-一定要在空格后 false-不一定在空格后
            std::optional<bool> isMustAfterWhiteSpace;
            //存储下一个节点，需要调用构造函数之后再进行添加
            std::vector<NodeBase *> nextNodes;

            NodeBase() = default;

            NodeBase(const std::optional<std::u16string> &id,
                     const std::optional<std::u16string> &description,
                     bool isMustAfterWhiteSpace);

            virtual ~NodeBase() = default;

            virtual void init(const CPack &cpack);

            [[nodiscard]] virtual NodeTypeId::NodeTypeId getNodeType() const = 0;

            [[nodiscard]] HEDLEY_NON_NULL(3) virtual ASTNode
                    getASTNode(TokenReader &tokenReader, const CPack *cpack) const = 0;

            [[nodiscard]] HEDLEY_NON_NULL(3) ASTNode
                    getASTNodeWithNextNode(TokenReader &tokenReader, const CPack *cpack) const;

            [[nodiscard]] HEDLEY_NON_NULL(3) ASTNode
                    getASTNodeWithNextNode(TokenReader &tokenReader, const CPack *cpack, bool isRequireWhitespace) const;

        protected:
            HEDLEY_NON_NULL(3, 4)
            ASTNode
            getByChildNode(TokenReader &tokenReader,
                           const CPack *cpack,
                           const NodeBase *childNode,
                           const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE) const;

            //node不一定需要的时侯使用
            HEDLEY_NON_NULL(3)
            ASTNode
            getOptionalASTNode(TokenReader &tokenReader,
                               const CPack *cpack,
                               bool isIgnoreChildNodesError,
                               const std::vector<const NodeBase *> &childNodes,
                               const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE) const;

        public:
            [[nodiscard]] bool isAfterWhitespace() const;

            HEDLEY_NON_NULL(2)
            virtual std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const;

            HEDLEY_NON_NULL(2)
            virtual bool collectIdError(const ASTNode *astNode,
                                        std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const;

            HEDLEY_NON_NULL(2)
            virtual bool collectSuggestions(const ASTNode *astNode,
                                            size_t index,
                                            std::vector<Suggestions> &suggestions) const;

            virtual void collectStructure(const ASTNode *astNode,
                                          StructureBuilder &structure,
                                          bool isMustHave) const;

            HEDLEY_NON_NULL(2)
            virtual bool collectColor(const ASTNode *astNode,
                                      ColoredString &coloredString,
                                      const Theme &theme) const;

            void collectStructureWithNextNodes(StructureBuilder &structure,
                                               bool isMustHave) const;
        };

    }// namespace Node

}// namespace CHelper

CODEC(CHelper::Node::NodeBase, id, brief, description, isMustAfterWhiteSpace)

template<>
struct serialization::Codec<std::unique_ptr<CHelper::Node::NodeBase>> : BaseCodec<std::unique_ptr<CHelper::Node::NodeBase>> {
    using Type = std::unique_ptr<CHelper::Node::NodeBase>;

    constexpr static bool enable = true;

    template<class JsonValueType>
    static void to_json(typename JsonValueType::AllocatorType &allocator,
                        JsonValueType &jsonValue,
                        const Type &t);

    template<class JsonValueType>
    static void from_json(const JsonValueType &jsonValue,
                          Type &t);

    template<bool isNeedConvert>
    static void to_binary(std::ostream &ostream,
                          const Type &t);

    template<bool isNeedConvert>
    static void from_binary(std::istream &istream,
                            Type &t);
};

#endif//CHELPER_NODEBASE_H
