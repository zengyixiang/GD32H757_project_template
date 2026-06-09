#ifndef CJSON_STUB_H
#define CJSON_STUB_H

typedef struct {
    int unused;
} cjson_doc_t;

cjson_doc_t *cjson_parse(const char *text);
void cjson_delete(cjson_doc_t *doc);

#endif
