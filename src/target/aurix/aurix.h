#ifndef OPENOCD_TARGET_AURIX_H
#define OPENOCD_TARGET_AURIX_H

#include <target/target.h>
#include <helper/command.h>

#include "aurix_ocds.h"


struct aurix_private_config {
  struct aurix_ocds *ocds;
  struct {
    int is_read;
    int is_write;
    int is_execute;
    uint32_t addr;
    int len;
    uint32_t mask;
    uint32_t value;
    int unique_id;
    int active;
  } hw_watch[AURIX_OCDS_MAX_HW_TRIGGERS];

  int trig_index[AURIX_OCDS_MAX_HW_TRIGGERS];
  
  int single_stepped;
};

static inline struct aurix_private_config *target_to_aurix(struct target *target) {
  return (struct aurix_private_config*) target->private_config;
}

struct tricore_reg {
  struct target *target;
  uint16_t offset;
  uint8_t value[4];
};
#endif
