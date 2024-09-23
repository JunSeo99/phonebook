#include "phonebook.h"
#include <stdio.h>

int main() {
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    mongoc_init();

    const char *username = getenv("MONGODB_USERNAME");
    const char *password = getenv("MONGODB_PASSWORD");
    printf("%s",username);
    printf("%s",password);
    char uri_string[256];
    snprintf(uri_string, sizeof(uri_string), "mongodb+srv://%s:%s@cluster0.mci9cwn.mongodb.net/test?retryWrites=true&w=majority", username, password);
    printf("%s",uri_string);
    bson_error_t error;
   
    client = mongoc_client_new(uri_string);
    if (!client) {
        fprintf(stderr, "URI 파싱 실패.\n");
        return EXIT_FAILURE;
    }

    collection = mongoc_client_get_collection(client, "phonebook", "phonebook");

    int choice;
    do {
        printf("\n--- 전화번호부 메뉴 ---\n");
        printf("1. 연락처 추가\n");
        printf("2. 연락처 삭제\n");
        printf("3. 연락처 검색\n");
        printf("4. 연락처 목록\n");
        printf("0. 종료\n");
        printf("선택하세요: ");
        scanf("%d", &choice);

        // 입력 버퍼 정리
        while (getchar() != '\n');

        switch(choice) {
            case 1:
                insert_contact(collection);
                break;
            case 2:
                delete_contact(collection);
                break;
            case 3:
                search_contact(collection);
                break;
            case 4:
                list_contacts(collection);
                break;
            case 0:
                printf("프로그램을 종료합니다.\n");
                break;
            default:
                printf("잘못된 선택입니다.\n");
        }
    } while(choice != 0);

    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return EXIT_SUCCESS;
}
