#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include <glib.h>

#ifndef __SEMVER_H__
#define __SEMVER_H__

/*
 * SEMVER VERSION
 */

typedef enum SemverVersionDifference {
  SemverVersionDifference_EQUAL,
  SemverVersionDifference_LESS,
  SemverVersionDifference_GREATER
} SemverVersionDifference;

typedef struct SemverVersion {
  unsigned int major;
  unsigned int minor;
  unsigned int patch;
  bool is_prerelease;
  char* prerelease_string;
} SemverVersion;

SemverVersion* semver_version_new(unsigned int major, unsigned int minor, unsigned int patch, bool is_prerelease, char* prerelease_string);
SemverVersion* semver_version_new_from_string(char* version_string);
void semver_version_free(SemverVersion* ver);
SemverVersion* semver_version_copy(SemverVersion* ver);
char* semver_version_to_string(SemverVersion* version);
SemverVersionDifference semver_version_compare(SemverVersion* ver1, SemverVersion* ver2);

/*
 * SEMVER FILTER
 */

typedef enum SemverFilterType {
  SemverFilterType_EQUAL,
  SemverFilterType_EXCLUDE,
  SemverFilterType_LESS_OR_EQUAL,
  SemverFilterType_GREATER_OR_EQUAL
} SemverFilterType;

typedef struct SemverFilter {
  SemverVersion* ver;
  SemverFilterType type;
} SemverFilter;

SemverFilter* semver_filter_new(SemverVersion* ver, SemverFilterType type);
SemverFilter* semver_filter_new_from_string(char* filter_string);
char* semver_filter_to_string(SemverFilter* filter);
bool semver_filter_matches_version(SemverFilter* filter, SemverVersion* version);

/*
 * SEMVER VERSION GLIST
 */

GList* semver_version_glist_apply_filters(GList* list, SemverFilter** filters);
GList* semver_version_glist_sort(GList* list);
GList* semver_version_glist_copy(GList* list);
void   semver_version_glist_free(GList* list);

#endif