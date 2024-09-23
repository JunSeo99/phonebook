#include "phonebook.h"
#include <stdio.h>
#include <string.h>

// 연락처 추가 기능
void insert_contact(mongoc_collection_t *collection) {
    bson_t *doc;
    bson_error_t error;
    char name[100];
    char phone[20];
    char department[100];
    
    printf("이름을 입력하세요: ");
    scanf("%99s", name);
    printf("전화번호를 입력하세요: ");
    scanf("%19s", phone);
    printf("부서를 입력하세요: ");
    scanf("%99s", department);

    doc = bson_new();
    BSON_APPEND_UTF8(doc, "name", name);
    BSON_APPEND_UTF8(doc, "phone", phone);
    BSON_APPEND_UTF8(doc, "department", department);

    if (!mongoc_collection_insert_one(collection, doc, NULL, NULL, &error)) {
        fprintf(stderr, "삽입 실패: %s\n", error.message);
    } else {
        printf("연락처가 성공적으로 추가되었습니다.\n");
    }

    bson_destroy(doc);
}

// 연락처 삭제 기능
void delete_contact(mongoc_collection_t *collection) {
    bson_t *query;
    bson_error_t error;
    char name[100];

    printf("삭제할 이름을 입력하세요: ");
    scanf("%99s", name);

    query = bson_new();
    BSON_APPEND_UTF8(query, "name", name);

    if (!mongoc_collection_delete_one(collection, query, NULL, NULL, &error)) {
        fprintf(stderr, "삭제 실패: %s\n", error.message);
    } else {
        printf("연락처가 성공적으로 삭제되었습니다.\n");
    }

    bson_destroy(query);
}

// 연락처 검색 기능 (이름 또는 전화번호로 검색)
void search_contact(mongoc_collection_t *collection) {
    bson_t *query;
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bson_error_t error;
    char search_term[100];

    printf("검색할 이름이나 부서를 입력하세요: ");
    scanf("%99s", search_term);

    // OR 조건을 사용하여 쿼리 생성
    query = BCON_NEW("$or",
        "[",
            "{", "name", BCON_UTF8(search_term), "}",
            "{", "department", BCON_UTF8(search_term), "}",
        "]"
    );

    cursor = mongoc_collection_find_with_opts(collection, query, NULL, NULL);

    bool found = false;
    while (mongoc_cursor_next(cursor, &doc)) {
        found = true;
        bson_iter_t iter;
        if (bson_iter_init(&iter, doc)) {
            printf("-------------------------------\n");
            while (bson_iter_next(&iter)) {
                const char *key = bson_iter_key(&iter);
                if (strcmp(key, "_id") != 0) { // _id 필드는 생략
                    if (BSON_ITER_HOLDS_UTF8(&iter)) {
                        printf("%s: %s\n", key, bson_iter_utf8(&iter, NULL));
                    }
                }
            }
        }
    }

    if (!found) {
        printf("일치하는 연락처가 없습니다.\n");
    }

    // 커서 오류 확인
    if (mongoc_cursor_error(cursor, &error)) {
        fprintf(stderr, "검색 중 오류 발생: %s\n", error.message);
    }

    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
}

// 연락처 목록 기능
void list_contacts(mongoc_collection_t *collection) {
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bson_error_t error;

    cursor = mongoc_collection_find_with_opts(collection, bson_new(), NULL, NULL);

    bool found = false;
    while (mongoc_cursor_next(cursor, &doc)) {
        found = true;
        bson_iter_t iter;
        if (bson_iter_init(&iter, doc)) {
            printf("-------------------------------\n");
            while (bson_iter_next(&iter)) {
                const char *key = bson_iter_key(&iter);
                if (strcmp(key, "_id") != 0) { // _id 필드는 생략
                    if (BSON_ITER_HOLDS_UTF8(&iter)) {
                        printf("%s: %s\n", key, bson_iter_utf8(&iter, NULL));
                    }
                }
            }
        }
    }

    if (!found) {
        printf("저장된 연락처가 없습니다.\n");
    }

    // 커서 오류 확인
    if (mongoc_cursor_error(cursor, &error)) {
        fprintf(stderr, "연락처 목록을 가져오는 중 오류 발생: %s\n", error.message);
    }

    mongoc_cursor_destroy(cursor);
}
