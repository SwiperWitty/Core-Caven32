#ifndef _GX_INFO_FRAME__H_
#define _GX_INFO_FRAME__H_

#ifdef CAVEN    /* 预编译 */
#include "Caven_Type.h"
#else
#include "stdint.h"
#include "string.h"
#include "stddef.h"
#include "stdio.h"
#endif

/*
    Caven_info_frame.h
    描述Caven的通信协议规范
    可能需要依赖外部的crc校验函数
*/

#define PROTOCOL_UHF_READER 0
#define PROTOCOL_ICIO_CMD 1
#define PROTOCOL_HUB_ANT 15

enum reader_order_MID
{
  query_reader_params_order = 0x00,
  query_reader_software_version_order = 0x01,
  config_reader_serial_params_order = 0x02,
  query_reader_serial_params_order = 0x03,
  config_reader_ip_address_params_order = 0x04,
  query_reader_ip_address_params_order = 0x05,
  query_reader_mac_address_params_order = 0x06,
  config_server_client_modle_params_order = 0x07,
  query_server_client_modle_params_order = 0x08,
  config_reader_GPO_state_order = 0x09,                 // !!!!
  query_reader_GPI_state_order = 0x0A,                  // !!!!
  config_reader_GPI_trigger_order = 0x0B,
  query_reader_GPI_trigger_order = 0x0C,
  config_reader_WG_params_order = 0x0D,                 // !!!!
  query_reader_WG_params_order = 0x0E,
  restart_reader_order = 0x0F,                          // !!!!
  config_reader_time_date_params_order = 0x10,
  query_reader_time_date_params_order = 0x11,
  reader_connect_state_confirm_order = 0x12,            // !!!!
  config_reader_mac_address_params_order = 0x13,
  reader_restore_default_params_order = 0x14,
  config_reader_rs485_address_order = 0x15,
  query_reader_rs485_addresss_order = 0x16,
  config_reader_breakpoint_continuation_order = 0x17,
  query_reader_breakpoint_continuation_order = 0x18,
  get_reader_6B_6C_tag_data_buffer_order = 0x1B,
  clear_reader_6B_6C_tag_data_buffer_order = 0x1C,
  tag_6B_6C_respeond_order = 0x1D, // 上位机收到带有应答标志的标签数据时候,发送此标签读取序号作为应答，配合断点续传功能使用
  beep_device_power_on_order = 0x1E,
  beep_device_control_order = 0x1F,
  get_reader_white_list_order = 0x20,
  import_reader_white_list_order = 0x21,
  del_reader_white_list_order = 0x22,
  config_reader_white_list_params_order = 0x23,
  get_reader_white_list_params_order = 0x24,
  config_reader_white_list_switch_order = 0x25,
  query_reader_white_list_switch_order = 0x26,
  config_reader_udp_send_data_params_order = 0x27,
  query_reader_udp_send_data_params_order = 0x28,
  config_reader_http_send_data_params_order = 0x29,
  query_reader_http_send_data_params_order = 0x2a,
  usb_keyboard_enable_or_disable_order = 0x2B,
  config_HL_company_reader_rj45_ip_params_order = 0x2F,
  query_HL_Company_reader_rj45_ip_params_order = 0x30,
  config_reader_wifi_start_scan_order = 0x31,
  query_reader_wifi_scan_result_order = 0x32,
  config_reader_wifi_ap_params_order = 0x33,
  query_reader_wifi_connect_state_order = 0x34,
  config_reader_wifi_network_params_order = 0x35,
  query_reader_wifi_network_params_order = 0x36,
  config_reader_wifi_enable_or_disable_order = 0x37,
  query_reader_wifi_enable_or_disable_order = 0x38,
  query_reader_hostname_order = 0x3e,
  config_reader_hostname_order = 0x3d,

  config_reader_EAS_order = 0x3F,
  query_reader_EAS_order = 0x40,
  config_sf_express_forklift_reader_order = 0x43,
  query_sf_express_forklift_reader_order = 0x44,
  config_wild_horse_ding_wei_entrance_guard_order = 0x45,
  query_wild_horse_ding_wei_entrance_guard_order = 0x46,
  config_sike_plc_tcp_params_reader_order = 0x47, // 配置斯科PLC-TCP业务配置
  query_sike_plc_tcp_params_reader_order = 0x48,
  config_nxp_antenna_control_order = 0x49,
  config_wifi_roaming_control_order = 0x4A,
  config_system_device_watch_dog_params_order = 0x4B,
  config_system_device_gps_params_order = 0x4C,
  query_system_device_gps_params_order = 0x4d,
  config_system_device_4G_params_order = 0x4E,
  query_system_device_IO_params_order = 0x4F,
  query_system_device_4G_params_order = 0x50,
  query_system_device_internet_connect_state_order = 0x51,
  config_system_device_internet_connect_sequence_order = 0x52,
  config_China_Constructiong_Bank_Corp_entrance_guard_order = 0x53,
  cofing_system_device_http_update_bin_order = 0x54,
  query_beep_set_parameter_for_HL_rfid_reader = 0x55, // 查询蜂鸣器的设置参数
  system_plug_in_device_send_data_order = 0x63,       // 外挂设备数据透传命令

  gsl_open_one_led_read_epc_order = 0x64, // 广速联定制读卡只开一个灯

  stop_read_tag_order = 0xff

};

/*
    此结构体包含指针变量，所以此结构体需要初始化其中的指针。
*/
typedef struct
{
    unsigned char Head;    //2byte

    unsigned char Prot_W_Type;      //1byte
    unsigned char Prot_W_Versions;  //1byte
    unsigned char Prot_W_485Type;   //1bit
    unsigned char Prot_W_DFlag;     //1bit
    unsigned char Prot_W_Class;     //4bit
    unsigned char Prot_W_MID;       //1byte

    unsigned char Addr;     //1byte 5

    unsigned short dSize;   //2byte 9
    unsigned char *p_Data;  // min 12byte buff

    unsigned short get_crc; //2byte
    unsigned short End_crc; //2byte

    unsigned char Result;   //1byte 
    int Run_status;
    unsigned short Get_num;

    unsigned char Comm_way;
}GX_info_packet_Type;

#ifndef BUFF_MAX
    #define BUFF_MAX  500
#endif // !BUFF_MAX

int GX_info_packet_clean_Fun(GX_info_packet_Type *target);
int GX_info_packet_fast_clean_Fun(GX_info_packet_Type *target);
int GX_info_packet_index_Fun(GX_info_packet_Type *target, unsigned char *data);
int GX_packet_data_copy_Fun(GX_info_packet_Type *source,GX_info_packet_Type *target);

int GX_Circular_queue_input (GX_info_packet_Type *data,GX_info_packet_Type *Buff_data,char Start_Num,char Buff_Num);
int GX_Circular_queue_output(GX_info_packet_Type *data,GX_info_packet_Type *Buff_data,char Buff_Num);

int GX_info_Make_packet_Fun(GX_info_packet_Type const standard, GX_info_packet_Type *target, unsigned char data);
int GX_info_rest_data_packet_Fun(GX_info_packet_Type *target, unsigned char *data,int Add_Num);

int GX_info_return_Fun (uint8_t cmd,uint8_t MID,uint8_t addr,uint8_t *data,uint16_t len,uint8_t *array);

void GX_info_remove_addr (GX_info_packet_Type *target);
void GX_info_add_addr (GX_info_packet_Type *target);

#endif
