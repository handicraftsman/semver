#include <stdio.h>
#include "semver.h"

char* verdiffstr(SemverVersionDifference diff) {
  switch (diff) {
    case SemverVersionDifference_GREATER: return "greater";
    case SemverVersionDifference_LESS: return "less";
    case SemverVersionDifference_EQUAL: return "equal";
  }
}

int main() {
  printf("--Versions\n");
  SemverVersion* ver1 = semver_version_new_from_string("1.0.3-asd");
  printf("%s\n", semver_version_to_string(ver1));
  SemverVersion* ver2 = semver_version_new_from_string("1.2.3");
  printf("%s\n", semver_version_to_string(ver2));

  printf("\n--Difference\n");
  printf("%s\n", verdiffstr(semver_version_compare(ver1, ver2)));

  printf("\n--Filter\n");
  SemverFilter* fil = semver_filter_new_from_string("1.0.3-asd");
  printf("%s %u\n", semver_filter_to_string(fil), semver_filter_matches_version(fil, ver1));
  
  GList* list = NULL;
  list = g_list_append(list, semver_version_new_from_string("0.0.1"));
  list = g_list_append(list, semver_version_new_from_string("0.1.1"));
  list = g_list_append(list, semver_version_new_from_string("0.1.1-beta.2"));
  list = g_list_append(list, semver_version_new_from_string("0.1.1-alpha.1"));
  list = g_list_append(list, semver_version_new_from_string("5.2.1"));
  list = g_list_append(list, semver_version_new_from_string("0.0.2"));
  list = g_list_append(list, semver_version_new_from_string("2.0.1"));
  list = g_list_append(list, semver_version_new_from_string("2.9.1"));
  
  GList* l;

  printf("\n--Before Sorting\n");
  l = list;
  while (l != NULL) {
    GList* next = l->next;
    printf("%s\n", semver_version_to_string(l->data));
    l = next;
  }

  list = semver_version_glist_sort(list);
  
  printf("\n--After Sorting\n");
  l = list;
  while (l != NULL) {
    GList* next = l->next;
    printf("%s\n", semver_version_to_string(l->data));
    l = next;
  }

  printf("\n--After applying a filter >=5.0.0\n");
  SemverFilter* filters[] = {
    semver_filter_new_from_string(">=5.0.0"),
    NULL
  };
  GList* list2 = semver_version_glist_apply_filters(semver_version_glist_copy(list), filters);
  l = list2;
  while (l != NULL) {
    GList* next = l->next;
    printf("%s\n", semver_version_to_string(l->data));
    l = next;
  }
  semver_version_glist_free(list2);

  printf("\n--After applying a filter >=2.0.0 <=3.0.0\n");
  SemverFilter* filters2[] = {
    semver_filter_new_from_string(">=2.0.0"),
    semver_filter_new_from_string("<=3.0.0"),
    NULL
  };
  GList* list3 = semver_version_glist_apply_filters(semver_version_glist_copy(list), filters2);
  l = list3;
  while (l != NULL) {
    GList* next = l->next;
    printf("%s\n", semver_version_to_string(l->data));
    l = next;
  }
  semver_version_glist_free(list3);

  semver_version_glist_free(list);
  return 0;
}