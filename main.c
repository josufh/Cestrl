#include <stdbool.h>
#include <stdlib.h>

#include "include/json.h"

int main(void) {
  const char *json = "[{\"name\":\"Joshua\",\"age\":29}, {\"name\":\"Xiaoli\",\"age\":34}]";
  JsonDocument *json_obj = JsonSerializer.Deserialize(json);
  JsonSerializer.Print(json_obj);

  return EXIT_SUCCESS;
}
