#include "semver.h"

#define SEMVER_VERSION_NEW_FROM_STRING_FAIL_INVALID_STRING(VER_STRING) \
  printf("Error: invalid version string: `%s`. Check correct syntax at http://semver->org/\n", VER_STRING);\
  exit(1);
#define SEMVER_FILTER_NEW_FROM_STRING_FAIL_INVALID_STRING(FIL_STRING) \
  printf("Error: invalid filter string: `%s`.\n", FIL_STRING);\
  exit(1);

/*
 * SEMVER VERSION
 */
SemverVersion* semver_version_new(unsigned int major, unsigned int minor, unsigned int patch, bool is_prerelease, char* prerelease_string) {
  SemverVersion* ver = (SemverVersion*) malloc(sizeof(SemverVersion));
  ver->major = major;
  ver->minor = minor;
  ver->patch = patch;
  ver->is_prerelease = is_prerelease;
  ver->prerelease_string = prerelease_string;
  return ver;
}

SemverVersion* semver_version_new_from_string(char* version_string) {
  SemverVersion* ver = (SemverVersion*) malloc(sizeof(SemverVersion));
  GRegex* regex = g_regex_new("^v?(\\d+)\\.(\\d+)\\.(\\d+)(?:-(.+))?$", 0, 0, NULL);
  GMatchInfo* match_info = NULL;
  bool ok = g_regex_match(regex, version_string, 0, &match_info);
  if (ok == false) {
    SEMVER_VERSION_NEW_FROM_STRING_FAIL_INVALID_STRING(version_string);
  }
  ver->is_prerelease = false;
  switch (g_match_info_get_match_count(match_info)-1) {
    case 4:
      ver->is_prerelease = true;
      ver->prerelease_string = g_match_info_fetch(match_info, 4);
    case 3:
      ver->major = atoi(g_match_info_fetch(match_info, 1));
      ver->minor = atoi(g_match_info_fetch(match_info, 2));
      ver->patch = atoi(g_match_info_fetch(match_info, 3));
      break;
    default:
      SEMVER_VERSION_NEW_FROM_STRING_FAIL_INVALID_STRING(version_string);
      break;
  }
  g_match_info_free(match_info);
  g_regex_unref(regex);
  return ver;
}

void semver_version_free(SemverVersion* ver) {
  if (ver->is_prerelease)
    free(ver->prerelease_string);
  free(ver);
}

SemverVersion* semver_version_copy(SemverVersion* ver) {
  SemverVersion* out = (SemverVersion*) malloc(sizeof(SemverVersion));
  out->major = ver->major;
  out->minor = ver->minor;
  out->patch = ver->patch;
  out->is_prerelease = ver->is_prerelease;
  if (out->is_prerelease) {
    out->prerelease_string = (char*) malloc(sizeof(char) * strlen(ver->prerelease_string));
    strcpy(out->prerelease_string, ver->prerelease_string);
  }
  return out;
}

char* semver_version_to_string(SemverVersion* ver) {
  size_t maj_len, min_len, pat_len;
  if (ver->major != 0) maj_len = (floor(log10(abs(ver->major))) + 1); else maj_len = 1;
  if (ver->minor != 0) min_len = (floor(log10(abs(ver->minor))) + 1); else min_len = 1;
  if (ver->patch != 0) pat_len = (floor(log10(abs(ver->patch))) + 1); else pat_len = 1;

  size_t len = 1;
  if (ver->is_prerelease)
    len += (3 + strlen(ver->prerelease_string));
  else
    len += 2;
  len += (maj_len + min_len + pat_len);

  char* str = (char*) malloc(sizeof(char) * (len));
  if (str == NULL) { printf("Error: cannot allocate %zu bytes of memory (semver.c: semver_version_to_string)\n", len); exit(1); }
  str[len] = '\0';
  if (ver->is_prerelease) {
    sprintf(str, "%u.%u.%u-%s", ver->major, ver->minor, ver->patch, ver->prerelease_string);
  } else {
    sprintf(str, "%u.%u.%u", ver->major, ver->minor, ver->patch);
  }

  return str;
}

SemverVersionDifference semver_version_compare(SemverVersion* ver1, SemverVersion* ver2) {
  if (ver1->major > ver2->major)
    return SemverVersionDifference_GREATER;
  if (ver1->major < ver2->major)
    return SemverVersionDifference_LESS;

  if (ver1->minor > ver2->minor)
    return SemverVersionDifference_GREATER;
  if (ver1->minor < ver2->minor)
    return SemverVersionDifference_LESS;

  if (ver1->patch > ver2->patch)
    return SemverVersionDifference_GREATER;
  if (ver1->patch < ver2->patch)
    return SemverVersionDifference_LESS;
  if (ver1->patch == ver2->patch && !ver1->is_prerelease && !ver2->is_prerelease)
    return SemverVersionDifference_EQUAL;

  if (!ver1->is_prerelease && ver2->is_prerelease)
    return SemverVersionDifference_GREATER;
  if (ver1->is_prerelease && !ver2->is_prerelease)
    return SemverVersionDifference_LESS;

  size_t l1 = strlen(ver1->prerelease_string);
  size_t l2 = strlen(ver2->prerelease_string);
  for (size_t i = 0; i < l1; i++) {
    if (i == l1-1)
      if (l2 > l1)
        return SemverVersionDifference_LESS;
    if (i == l2-1)
      if (l1 > l2)
        return SemverVersionDifference_GREATER;

    if (isdigit(ver1->prerelease_string[i]) && !isdigit(ver2->prerelease_string[i]))
      return SemverVersionDifference_LESS;
    if (!isdigit(ver1->prerelease_string[i]) && isdigit(ver2->prerelease_string[i]))
      return SemverVersionDifference_GREATER;

    if (ver1->prerelease_string[i] < ver2->prerelease_string[i])
      return SemverVersionDifference_LESS;
    if (ver1->prerelease_string[i] > ver2->prerelease_string[i])
      return SemverVersionDifference_GREATER;
  }

  return SemverVersionDifference_EQUAL;
}

/*
 * SEMVER FILTER
 */

SemverFilter* semver_filter_new(SemverVersion* ver, SemverFilterType type) {
  SemverFilter* filter = (SemverFilter*) malloc(sizeof(SemverFilter));
  filter->ver  = ver;
  filter->type = type;
  return filter;
}

SemverFilter* semver_filter_new_from_string(char* filter_string) {
  SemverFilter* filter = (SemverFilter*) malloc(sizeof(SemverFilter));
  GRegex* regex = g_regex_new("^(==|>=|<=|!=)?(.+)$", 0, 0, NULL);
  GMatchInfo* match_info = NULL;
  bool ok = g_regex_match(regex, filter_string, 0, &match_info);
  if (ok == false) {
    SEMVER_FILTER_NEW_FROM_STRING_FAIL_INVALID_STRING(filter_string);
  }
  filter->ver = semver_version_new_from_string(g_match_info_fetch(match_info, 2));
  char* f_str = g_match_info_fetch(match_info, 1);
  if (strlen(f_str) == 0)
    filter->type = SemverFilterType_EQUAL;
  else if (strcmp(f_str, "==") == 0)
    filter->type = SemverFilterType_EQUAL;
  else if (strcmp(f_str, "!=") == 0)
    filter->type = SemverFilterType_EXCLUDE;
  else if (strcmp(f_str, "<=") == 0)
    filter->type = SemverFilterType_LESS_OR_EQUAL;
  else if (strcmp(f_str, ">=") == 0)
    filter->type = SemverFilterType_GREATER_OR_EQUAL;
  else {
    SEMVER_FILTER_NEW_FROM_STRING_FAIL_INVALID_STRING(filter_string);
  }
  g_match_info_free(match_info);
  g_regex_unref(regex);
  return filter;
}

char* semver_filter_to_string(SemverFilter* filter) {
  char* typechars;
  switch (filter->type) {
    case SemverFilterType_EQUAL:            typechars = "=="; break;
    case SemverFilterType_EXCLUDE:          typechars = "!="; break;
    case SemverFilterType_LESS_OR_EQUAL:    typechars = "<="; break;
    case SemverFilterType_GREATER_OR_EQUAL: typechars = ">="; break;
  }
  char* verchars = semver_version_to_string(filter->ver);
  char* str = (char*) malloc(sizeof(char) * (3 + strlen(verchars)));
  sprintf(str, "%s%s", typechars, verchars);
  free(verchars);
  return str;
}

bool semver_filter_matches_version(SemverFilter* filter, SemverVersion* ver) {
  SemverVersionDifference diff = semver_version_compare(ver, filter->ver);
  switch (filter->type) {
    case SemverFilterType_EQUAL:
      if (diff == SemverVersionDifference_EQUAL)
        return true;
      else
        return false;
    case SemverFilterType_EXCLUDE:
      if (diff != SemverVersionDifference_EQUAL)
        return true;
      else
        return false;
    case SemverFilterType_LESS_OR_EQUAL:
      if ((diff == SemverVersionDifference_LESS) || (diff == SemverVersionDifference_EQUAL))
        return true;
      else
        return false;
    case SemverFilterType_GREATER_OR_EQUAL:
      if ((diff == SemverVersionDifference_GREATER) || (diff == SemverVersionDifference_EQUAL))
        return true;
      else
        return false;
    default:
      return false;
  }
}

/* 
 * SEMVER VERSION GLIST
 */

GList* semver_version_glist_apply_filters(GList* list_, SemverFilter** filters) {
  GList* list = list_;
  GList* l = list;

  while (l != NULL) {
    GList* next = l->next;
    int not_matches = 0;
    for (int f = 0; filters[f] != NULL; f++) {
      if (!semver_filter_matches_version(filters[f], (SemverVersion*) l->data))
        not_matches++;
    }
    if (not_matches) {
      semver_version_free(l->data);
      list = g_list_delete_link(list, l);
    }
    l = next;
  }
  return list;
}

int _semver_version_glist_sort_func(const void* ver1_, const void* ver2_) {
  SemverVersion* ver1 = (SemverVersion*) ver1_;
  SemverVersion* ver2 = (SemverVersion*) ver2_;
  SemverVersionDifference diff = semver_version_compare(ver1, ver2);
  switch (diff) {
    case SemverVersionDifference_EQUAL:   return  0;
    case SemverVersionDifference_LESS:    return -1;
    case SemverVersionDifference_GREATER: return  1;
  }
}

GList* semver_version_glist_sort(GList* list_) {
  GList* list = list_;
  list = g_list_sort(list, &_semver_version_glist_sort_func);
  return list;
}

void* _semver_version_glist_copy_func(const void* ver_, gpointer data) {
  SemverVersion* ver = (SemverVersion*) ver_;
  return semver_version_copy(ver);
}

GList* semver_version_glist_copy(GList* list_) {
  GList* list1 = list_;
  GList* list2;
  list2 = g_list_copy_deep(list1, &_semver_version_glist_copy_func, NULL);
  return list2;
}

void _semver_version_glist_free_func(gpointer ver) {
  semver_version_free((SemverVersion*) ver);
}

void semver_version_glist_free(GList* list) {
  g_list_free_full(list, &_semver_version_glist_free_func);
}