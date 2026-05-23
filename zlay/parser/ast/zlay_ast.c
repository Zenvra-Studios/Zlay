#include <parser/ast/zlay_ast.h>

#include <string.h>

static void* ZLay__ParserAlloc(ZLay_Arena* arena, size_t size) {
  void* memory = ZLay_ArenaAlloc(arena, size, sizeof(void*));
  if (memory != NULL) {
    memset(memory, 0, size);
  }
  return memory;
}

bool ZLay_StringEquals(ZLay_String a, ZLay_String b) {
  if (a.length != b.length) {
    return false;
  }
  if (a.length == 0u) {
    return true;
  }
  if (a.chars == NULL || b.chars == NULL) {
    return false;
  }
  return memcmp(a.chars, b.chars, a.length) == 0;
}

bool ZLay_StringEqualsCString(ZLay_String a, const char* b) {
  uint32_t index = 0u;
  if (b == NULL) {
    return false;
  }
  while (b[index] != '\0') {
    if (index >= a.length || a.chars == NULL || a.chars[index] != b[index]) {
      return false;
    }
    ++index;
  }
  return index == a.length;
}

ZLay_String ZLay_ParserStringFromCString(const char* cstr) {
  ZLay_String result;
  result.chars = cstr;
  result.length = 0u;
  result.is_static = true;
  if (cstr == NULL) {
    return result;
  }
  while (cstr[result.length] != '\0') {
    ++result.length;
  }
  return result;
}

ZLay_String ZLay_ArenaCopyString(ZLay_Arena* arena, ZLay_String source) {
  ZLay_String copy;
  char* chars;

  copy.chars = NULL;
  copy.length = 0u;
  copy.is_static = false;

  if (source.length == 0u) {
    copy.chars = "";
    copy.is_static = true;
    return copy;
  }

  chars = (char*)ZLay_ArenaAlloc(arena, (size_t)source.length + 1u, 1u);
  if (chars == NULL) {
    return copy;
  }

  memcpy(chars, source.chars, source.length);
  chars[source.length] = '\0';
  copy.chars = chars;
  copy.length = source.length;
  copy.is_static = false;
  return copy;
}

ZLay_AstNode* ZLay_AstCreateNode(
  ZLay_Arena* arena,
  ZLay_String type,
  uint32_t line,
  uint32_t column
) {
  ZLay_AstNode* node = (ZLay_AstNode*)ZLay__ParserAlloc(arena, sizeof(ZLay_AstNode));
  if (node == NULL) {
    return NULL;
  }

  node->type = ZLay_ArenaCopyString(arena, type);
  if (node->type.chars == NULL && type.length != 0u) {
    return NULL;
  }
  node->line = line;
  node->column = column;
  return node;
}

static bool ZLay__AstGrowProps(ZLay_Arena* arena, ZLay_AstNode* node) {
  uint32_t new_capacity;
  ZLay_AstProp* props;

  if (node == NULL) {
    return false;
  }

  if (node->prop_count < node->prop_capacity) {
    return true;
  }

  new_capacity = node->prop_capacity == 0u ? 4u : node->prop_capacity * 2u;
  props = (ZLay_AstProp*)ZLay__ParserAlloc(arena, sizeof(ZLay_AstProp) * (size_t)new_capacity);
  if (props == NULL) {
    return false;
  }

  if (node->props != NULL && node->prop_count != 0u) {
    memcpy(props, node->props, sizeof(ZLay_AstProp) * (size_t)node->prop_count);
  }

  node->props = props;
  node->prop_capacity = new_capacity;
  return true;
}

static bool ZLay__AstGrowChildren(ZLay_Arena* arena, ZLay_AstNode* node) {
  uint32_t new_capacity;
  ZLay_AstNode* children;

  if (node == NULL) {
    return false;
  }

  if (node->child_count < node->child_capacity) {
    return true;
  }

  new_capacity = node->child_capacity == 0u ? 4u : node->child_capacity * 2u;
  children = (ZLay_AstNode*)ZLay__ParserAlloc(arena, sizeof(ZLay_AstNode) * (size_t)new_capacity);
  if (children == NULL) {
    return false;
  }

  if (node->children != NULL && node->child_count != 0u) {
    memcpy(children, node->children, sizeof(ZLay_AstNode) * (size_t)node->child_count);
  }

  node->children = children;
  node->child_capacity = new_capacity;
  return true;
}

bool ZLay_AstAddProp(
  ZLay_Arena* arena,
  ZLay_AstNode* node,
  ZLay_String name,
  ZLay_Value value,
  uint32_t line,
  uint32_t column
) {
  ZLay_AstProp* prop;

  if (!ZLay__AstGrowProps(arena, node)) {
    return false;
  }

  prop = &node->props[node->prop_count++];
  prop->name = ZLay_ArenaCopyString(arena, name);
  if (prop->name.chars == NULL && name.length != 0u) {
    return false;
  }
  if (value.type == ZLAY_VALUE_STRING || value.type == ZLAY_VALUE_IDENT) {
    value.string = ZLay_ArenaCopyString(arena, value.string);
    if (value.string.chars == NULL && value.string.length != 0u) {
      return false;
    }
  }
  prop->value = value;
  prop->line = line;
  prop->column = column;
  return true;
}

ZLay_AstNode* ZLay_AstAddChild(
  ZLay_Arena* arena,
  ZLay_AstNode* parent,
  ZLay_String type,
  uint32_t line,
  uint32_t column
) {
  ZLay_AstNode* child;

  if (!ZLay__AstGrowChildren(arena, parent)) {
    return NULL;
  }

  child = &parent->children[parent->child_count++];
  memset(child, 0, sizeof(*child));
  child->type = ZLay_ArenaCopyString(arena, type);
  if (child->type.chars == NULL && type.length != 0u) {
    return NULL;
  }
  child->line = line;
  child->column = column;
  return child;
}

const ZLay_AstProp* ZLay_AstFindProp(const ZLay_AstNode* node, const char* name) {
  if (node == NULL || name == NULL) {
    return NULL;
  }

  for (uint32_t i = 0u; i < node->prop_count; ++i) {
    if (ZLay_StringEqualsCString(node->props[i].name, name)) {
      return &node->props[i];
    }
  }

  return NULL;
}
