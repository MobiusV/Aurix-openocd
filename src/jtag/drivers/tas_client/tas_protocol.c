#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "helper/log.h"
#include "tas_pkt.h"
#include "tas_sock.h"
 
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
 
int tas_client_connect(int sock) {
  tas_pl1rq_server_connect_st rq_server_connect;
  tas_pl1rsp_server_connect_st rsp_server_connect;
  uint32_t packet_size;

  packet_size = 4 + sizeof(tas_pl1rq_server_connect_st);
  rq_server_connect.wl = sizeof(tas_pl1rq_server_connect_st) / 4 - 1;
  rq_server_connect.cmd = TAS_PL1_CMD_SERVER_CONNECT;
  rq_server_connect.reserved = 0;
  snprintf(rq_server_connect.client_name, TAS_NAME_LEN32, "openocd");
  tas_get_login(rq_server_connect.user_name, TAS_NAME_LEN16);
  rq_server_connect.client_pid = tas_get_pid();
 
  if (tas_sock_send(sock, &packet_size, 4, 0) < 0) {
  	return ERROR_FAIL;
  }
  if (tas_sock_send(sock, &rq_server_connect, sizeof(tas_pl1rq_server_connect_st), 0) <
  	0) {
  	return ERROR_FAIL;
  }
 
  if (tas_sock_recv(sock, &packet_size, 4, 0) != 4) {
  	return ERROR_FAIL;
  }
  if (tas_sock_recv(sock, &rsp_server_connect, sizeof(tas_pl1rsp_server_connect_st), 0) <
  	0) {
    return ERROR_FAIL;
  }

  if (rsp_server_connect.cmd != TAS_PL1_CMD_SERVER_CONNECT ||
      rsp_server_connect.err != TAS_PL_ERR_NO_ERROR) {
    return ERROR_FAIL;
  }

  return 0;
}

int tas_client_session_start(int sock, const char *device, uint8_t con_id,
                             tas_con_info_st *con_info) {
  tas_pl1rq_session_start_st rq_session_start;
  tas_pl1rsp_session_start_st rsp_session_start;
  uint32_t packet_size;

  packet_size = 4 + sizeof(tas_pl1rq_session_start_st);
  rq_session_start.wl = sizeof(tas_pl1rq_session_start_st) / 4 - 1;
  rq_session_start.cmd = TAS_PL1_CMD_SESSION_START;
  rq_session_start.con_id = con_id;
  rq_session_start.client_type = TAS_CLIENT_TYPE_RW;
  strncpy(rq_session_start.identifier, device, TAS_NAME_LEN64 - 1);
    /* Ensure null termination */
    rq_session_start.identifier[TAS_NAME_LEN64 - 1] = '\0';
  snprintf(rq_session_start.session_name, TAS_NAME_LEN16, "openocd%u", con_id);
  rq_session_start.session_pw[0] = 0;

  if (tas_sock_send(sock, &packet_size, 4, 0) < 0) {
  	return ERROR_FAIL;
  }
  if (tas_sock_send(sock, &rq_session_start, sizeof(tas_pl1rq_session_start_st), 0) <
  	0) {
  	return ERROR_FAIL;
  }
 
  if (tas_sock_recv(sock, &packet_size, 4, 0) != 4) {
  	return ERROR_FAIL;
  }
  if (tas_sock_recv(sock, &rsp_session_start, sizeof(tas_pl1rsp_session_start_st), 0) <
  	0) {
    return ERROR_FAIL;
  }

  if (rsp_session_start.cmd != TAS_PL1_CMD_SESSION_START ||
      rsp_session_start.con_id != con_id ||
      rsp_session_start.err != TAS_PL_ERR_NO_ERROR) {
    return ERROR_FAIL;
  }

  if (rsp_session_start.num_instances > 0) {
    return ERROR_FAIL;
  }
  *con_info = rsp_session_start.con_info;

  return 0;
}

int tas_client_device_connect(int sock, tas_dev_con_feat_et dev_con_feat) {
  tas_pl1rq_device_connect_st rq_device_connect;
  tas_pl1rsp_device_connect_st rsp_device_connect;
  uint32_t packet_size;

  packet_size = 4 + sizeof(tas_pl1rq_device_connect_st);
  rq_device_connect.wl = sizeof(tas_pl1rq_device_connect_st) / 4 - 1;
  rq_device_connect.cmd = TAS_PL1_CMD_DEVICE_CONNECT;
  rq_device_connect.con_id = 0xFF;
  rq_device_connect.reserved = 0;
  rq_device_connect.option = dev_con_feat;
  rq_device_connect.reserved1 = 0;

  if (tas_sock_send(sock, &packet_size, 4, 0) < 0) {
  	return ERROR_FAIL;
  }
  if (tas_sock_send(sock, &rq_device_connect, sizeof(tas_pl1rq_device_connect_st), 0) <
  	0) {
  	return ERROR_FAIL;
  }
 
  if (tas_sock_recv(sock, &packet_size, 4, 0) != 4) {
  	return ERROR_FAIL;
  }
  if (tas_sock_recv(sock, &rsp_device_connect, sizeof(tas_pl1rsp_device_connect_st), 0) <
  	0) {
    return ERROR_FAIL;
  }

  if (rsp_device_connect.cmd != TAS_PL1_CMD_DEVICE_CONNECT ||
      rsp_device_connect.err != TAS_PL_ERR_NO_ERROR) {
    return ERROR_FAIL;
  }

  if (rsp_device_connect.feat_used != dev_con_feat) {
    return ERROR_FAIL;
  }

  return 0;
}

int tas_client_get_targets(int sock, tas_target_info_st **targets,
                           size_t *target_num) {
  tas_pl1rq_get_targets_st rq_get_targets;
  tas_pl1rsp_get_targets_st rsp_get_targets;
  uint32_t packet_size;
  if (targets == NULL) {
    return ERROR_FAIL;
  }

  packet_size = 4 + sizeof(tas_pl1rq_get_targets_st);
  rq_get_targets.cmd = TAS_PL1_CMD_GET_TARGETS;
  rq_get_targets.wl = 0;
  rq_get_targets.start_index = 0;

  if (tas_sock_send(sock, &packet_size, 4, 0) < 0) {
  	return ERROR_FAIL;
  }
  if (tas_sock_send(sock, &rq_get_targets, sizeof(tas_pl1rq_get_targets_st), 0) < 0) {
  	return ERROR_FAIL;
  }
 
  if (tas_sock_recv(sock, &packet_size, 4, 0) != 4) {
  	return ERROR_FAIL;
  }
  if (tas_sock_recv(sock, &rsp_get_targets, sizeof(tas_pl1rsp_get_targets_st), 0) < 0) {
  	return ERROR_FAIL;
  }

  if (rsp_get_targets.cmd != TAS_PL1_CMD_GET_TARGETS ||
      rsp_get_targets.err != TAS_PL_ERR_NO_ERROR) {
    return ERROR_FAIL;
  }

  *target_num = rsp_get_targets.num_target;
  /* Limit number of targets supported */
  if (*target_num > 32) {
    return ERROR_FAIL;
  }
  if (*target_num > 0) {
    *targets = calloc(*target_num, sizeof(tas_target_info_st));
    if (*targets == NULL) {
      return ERROR_FAIL;
    }
    if (tas_sock_recv(sock, *targets, *target_num * sizeof(tas_target_info_st), 0) < 0) {
    	return ERROR_FAIL;
    }
   }

  return 0;
}

enum {
  PROTOC_VER = 0 //!< \brief TasPkt protocol version implemented in this class
};

static uint16_t pl1_count = 0;

struct tas_client_pl0_req {
  uint32_t addr;
  uint8_t *buffer;
  uint8_t cmd;
};

int tas_client_send_pl0(int sock, uint8_t con_id, uint32_t *pl0_buffer,
                        size_t pl0_len, size_t pl0_elements) {

  uint32_t packet_size = 4 + sizeof(tas_pl1rq_pl0_start_st) +
                         sizeof(tas_pl1rq_pl0_end_st) + pl0_len;
  tas_pl1rq_pl0_start_st rq_start = {
      .cmd = TAS_PL1_CMD_PL0_START,
      .wl = 0,
      .con_id = con_id,
      .pl0_addr_map_mask = 1,
      .pl1_cnt = pl1_count++,
      .protoc_ver = PROTOC_VER,

  };
  tas_pl1rq_pl0_end_st rq_end = {
      .wl = 0, .cmd = TAS_PL1_CMD_PL0_END, .num_pl0_rw = pl0_elements};
  tas_pl1rsp_pl0_start_st rsp_start;
  tas_pl1rsp_pl0_end_st rsp_end;

  if (tas_sock_send(sock, &packet_size, 4, 0) < 0) {
  	return ERROR_FAIL;
  }
  if (tas_sock_send(sock, &rq_start, sizeof(tas_pl1rq_pl0_start_st), 0) < 0) {
  	return ERROR_FAIL;
  }
  if (tas_sock_send(sock, pl0_buffer, pl0_len, 0) < 0) {
  	return ERROR_FAIL;
  }
  if (tas_sock_send(sock, &rq_end, sizeof(tas_pl1rq_pl0_end_st), 0) < 0) {
  	return ERROR_FAIL;
  }
 
  if (tas_sock_recv(sock, &packet_size, 4, 0) != 4) {
  	return ERROR_FAIL;
  }
  if (tas_sock_recv(sock, &rsp_start, sizeof(tas_pl1rsp_pl0_start_st), 0) < 0) {
    return ERROR_FAIL;
  }

  if (rsp_start.cmd != TAS_PL1_CMD_PL0_START ||
      (rsp_start.err != TAS_PL_ERR_NO_ERROR &&
       rsp_start.err != TAS_PL_ERR_PROTOCOL)) {
    uint8_t buf[packet_size - 4 - sizeof(tas_pl1rsp_pl0_start_st)];
    tas_sock_recv(sock, buf, packet_size - 4 - sizeof(tas_pl1rsp_pl0_start_st), 0);
    return ERROR_FAIL;
   }
   size_t recv_pl0_len = packet_size - 4 - sizeof(tas_pl1rsp_pl0_start_st) -
                         sizeof(tas_pl1rsp_pl0_end_st);

   /* SECURITY: Read only up to the buffer's capacity to prevent overflow. */
   size_t bytes_to_copy = MIN(recv_pl0_len, pl0_len);

   int err = tas_sock_recv(sock, pl0_buffer, bytes_to_copy, 0);
   if (err < 0) {
     return ERROR_FAIL;
   };

   /* Drain any remaining data from the packet that didn't fit in the buffer. */
   size_t remaining_bytes_in_packet = recv_pl0_len - err;
   if (recv_pl0_len > pl0_len) {
     LOG_WARNING("Server sent more data (%zu) than buffer capacity (%zu). Truncating.",
                 recv_pl0_len, pl0_len);
   }

   while (remaining_bytes_in_packet > 0) {
     uint8_t drain_buf[1024];
     size_t bytes_to_drain = MIN(sizeof(drain_buf), remaining_bytes_in_packet);
     int drain_err = tas_sock_recv(sock, drain_buf, bytes_to_drain, 0);
     if (drain_err <= 0) {
       LOG_ERROR("Socket error while draining packet.");
       return ERROR_FAIL;
     }
     remaining_bytes_in_packet -= drain_err;
   }
  
   if (tas_sock_recv(sock, &rsp_end, sizeof(tas_pl1rsp_pl0_end_st), 0) < 0) {
    return ERROR_FAIL;
  }
  if (rsp_end.cmd != TAS_PL1_CMD_PL0_END ||
      rsp_end.pl1_cnt != rq_start.pl1_cnt) {
    return ERROR_FAIL;
  }

  return ERROR_OK;
}
