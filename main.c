#include <stdbool.h>
#include <stdlib.h>

#include "include/json.h"

int main(void) {
  const char *json = "-420.69e-3";
  JsonDocument *json_obj = JsonSerializer.Deserialize(json);

  return EXIT_SUCCESS;
}
