#include <stdbool.h>
#include <stdlib.h>

#include "include/json.h"

int main(void) {
  const char *json = "{\"name\":\"Joshua\",\"age\":29}";
  JsonDocument *json_obj = JsonSerializer.Deserialize(json);

  return EXIT_SUCCESS;
}
