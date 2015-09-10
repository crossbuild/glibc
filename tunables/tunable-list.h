/* Print a full tunable enum name.  */
#define TUNABLE_ENUM_NAME(top,ns,id) TUNABLE_ENUM_NAME1 (top,ns,id)
#define TUNABLE_ENUM_NAME1(top,ns,id) top ## _ ## ns ## _ ## id

/* Full name for a tunable is top_ns.tunable_ns.id.  */
#define TUNABLE_NAME_S(top,ns,id) #top "." #ns "." #id

typedef enum
{
  TUNABLE_ENUM_NAME(glibc, malloc, check),
  TUNABLE_ENUM_NAME(glibc, malloc, top_pad),
  TUNABLE_ENUM_NAME(glibc, malloc, perturb),
  TUNABLE_ENUM_NAME(glibc, malloc, mmap_threshold),
  TUNABLE_ENUM_NAME(glibc, malloc, trim_threshold),
  TUNABLE_ENUM_NAME(glibc, malloc, mmap_max),
  TUNABLE_ENUM_NAME(glibc, malloc, arena_max),
  TUNABLE_ENUM_NAME(glibc, malloc, arena_test),
} tunable_id_t;

#ifdef TUNABLES_INTERNAL
static tunable_t tunable_list[] = {
  {TUNABLE_NAME_S(glibc, malloc, check), NULL, NULL, false},
  {TUNABLE_NAME_S(glibc, malloc, top_pad), NULL, NULL, false},
  {TUNABLE_NAME_S(glibc, malloc, perturb), NULL, NULL, false},
  {TUNABLE_NAME_S(glibc, malloc, mmap_threshold), NULL, NULL, false},
  {TUNABLE_NAME_S(glibc, malloc, trim_threshold), NULL, NULL, false},
  {TUNABLE_NAME_S(glibc, malloc, mmap_max), NULL, NULL, false},
  {TUNABLE_NAME_S(glibc, malloc, arena_max), NULL, NULL, false},
  {TUNABLE_NAME_S(glibc, malloc, arena_test), NULL, NULL, false},
};
#endif