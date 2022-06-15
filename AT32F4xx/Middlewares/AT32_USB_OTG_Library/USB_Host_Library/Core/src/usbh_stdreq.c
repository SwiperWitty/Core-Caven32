/**
  ******************************************************************************
  * File   : usbh_stdreq.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file implements the standard requests for device enumeration
  ******************************************************************************
  */   
/* Includes ------------------------------------------------------------------*/

#include "usbh_ioreq.h"
#include "usbh_stdreq.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_LIB_CORE
* @{
*/

/** @defgroup USBH_STDREQ 
* @brief This file implements the standard requests for device enumeration
* @{
*/


/** @defgroup USBH_STDREQ_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_STDREQ_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 



/** @defgroup USBH_STDREQ_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_STDREQ_Private_Variables
* @{
*/
/**
* @}
*/ 
uint8_t USB_HOST_CfgDesc[CFG_DESC_MAX_SIZE];


/** @defgroup USBH_STDREQ_Private_FunctionPrototypes
* @{
*/
static void USB_HOST_ParseDevDesc (USB_HOST_DevDesDesc_Type* , uint8_t *buf, uint16_t length);

USB_HOST_Status USB_HOST_ParseCfgDesc (USB_HOST_CfgDesc_Type* cfg_desc,
                               USB_HOST_InterfaceDesc_Type* itf_desc,
                               USB_HOST_EpDesc_Type  ep_desc[][USBH_MAX_NUM_ENDPOINTS],                                                           
                               uint8_t *buf, 
                               uint16_t length);


static void USB_HOST_ParseInterfaceDesc (USB_HOST_InterfaceDesc_Type  *if_descriptor, uint8_t *buf);
static void USB_HOST_ParseEPDesc (USB_HOST_EpDesc_Type  *ep_descriptor, uint8_t *buf);

static void USB_HOST_ParseStringDesc (uint8_t* psrc, uint8_t* pdest, uint16_t length);
/**
* @}
*/ 


/** @defgroup USBH_STDREQ_Private_Functions
* @{
*/ 


/**
* @brief  USB_HOST_Get_DevDesc
*         Issue Get Device Descriptor command to the device. Once the response 
*         received, it parses the device descriptor and updates the status.
* @param  pusbdev: Selected device
* @param  dev_desc: Device Descriptor buffer address
* @param  pusbdev->host.Rx_Buffer: Receive Buffer address
* @param  length: Length of the descriptor
* @retval Status
*/
USB_HOST_Status USB_HOST_Get_DevDesc(USB_OTG_CORE_HANDLE *pusbdev,
                             USB_HOST *phost,
                             uint8_t length)
{
  
  USB_HOST_Status status;
  
  if((status = USB_HOST_GetDescriptor(pusbdev, 
                                  phost,
                                  USB_REQUEST_RECIPIENT_DEVICE | USB_REQUEST_TYPE_STANDARD,                          
                                  USB_DESC_DEVICE, 
                                  pusbdev->host.Rx_Buffer,
                                  length)) == USB_HOST_OK)
  {
    /* Commands successfully sent and Response Received */       
    USB_HOST_ParseDevDesc(&phost->device_prop.Dev_Desc, pusbdev->host.Rx_Buffer, length);
  }
  return status;      
}

/**
* @brief  USB_HOST_Get_CfgDesc
*         Issues Configuration Descriptor to the device. Once the response 
*         received, it parses the configuration descriptor and updates the 
*         status.
* @param  pusbdev: Selected device
* @param  cfg_desc: Configuration Descriptor address
* @param  itf_desc: Interface Descriptor address
* @param  ep_desc: Endpoint Descriptor address
* @param  length: Length of the descriptor
* @retval Status
*/
USB_HOST_Status USB_HOST_Get_CfgDesc(USB_OTG_CORE_HANDLE *pusbdev, 
                             USB_HOST           *phost,                      
                             uint16_t length)

{
  USB_HOST_Status status, result;
  uint16_t index = 0;
  
  if((status = USB_HOST_GetDescriptor(pusbdev,
                                  phost,
                                  USB_REQUEST_RECIPIENT_DEVICE | USB_REQUEST_TYPE_STANDARD,                          
                                  USB_DESC_CONFIGURATION, 
                                  pusbdev->host.Rx_Buffer,
                                  length)) == USB_HOST_OK)
  {
    /*save Cfg descriptor for class parsing usage */
    for( ; index < length ; index ++)
    {
      USB_HOST_CfgDesc[index] = pusbdev->host.Rx_Buffer[index];
    }
    
    /* Commands successfully sent and Response Received  */       
    result = USB_HOST_ParseCfgDesc (&phost->device_prop.Cfg_Desc,
                       phost->device_prop.Itf_Desc,
                       phost->device_prop.Ept_Desc, 
                       pusbdev->host.Rx_Buffer,
                       length);
    if (result != USB_HOST_OK)  status= result;
     
    
  }
  return status;
}


/**
* @brief  USB_HOST_Get_StringDesc
*         Issues string Descriptor command to the device. Once the response 
*         received, it parses the string descriptor and updates the status.
* @param  pusbdev: Selected device
* @param  string_index: String index for the descriptor
* @param  buff: Buffer address for the descriptor
* @param  length: Length of the descriptor
* @retval Status
*/
USB_HOST_Status USB_HOST_Get_StringDesc(USB_OTG_CORE_HANDLE *pusbdev,
                                USB_HOST *phost,
                                uint8_t string_index, 
                                uint8_t *buff, 
                                uint16_t length)
{
  USB_HOST_Status status;
  
  if((status = USB_HOST_GetDescriptor(pusbdev,
                                  phost,
                                  USB_REQUEST_RECIPIENT_DEVICE | USB_REQUEST_TYPE_STANDARD,                                    
                                  USB_DESC_STRING | string_index, 
                                  pusbdev->host.Rx_Buffer,
                                  length)) == USB_HOST_OK)
  {
    /* Commands successfully sent and Response Received  */       
    USB_HOST_ParseStringDesc(pusbdev->host.Rx_Buffer,buff, length);    
  }
  return status;
}

/**
* @brief  USB_HOST_GetDescriptor
*         Issues Descriptor command to the device. Once the response received,
*         it parses the descriptor and updates the status.
* @param  pusbdev: Selected device
* @param  req_type: Descriptor ept_type
* @param  value_idx: wValue for the GetDescriptr request
* @param  buff: Buffer to store the descriptor
* @param  length: Length of the descriptor
* @retval Status
*/
USB_HOST_Status USB_HOST_GetDescriptor(USB_OTG_CORE_HANDLE *pusbdev,
                               USB_HOST           *phost,                                
                               uint8_t  req_type,
                               uint16_t value_idx, 
                               uint8_t* buff, 
                               uint16_t length )
{ 
  phost->Control.setup.b.bmRequestType = USB_DIR_D2H | req_type;
  phost->Control.setup.b.bRequest = USB_REQUEST_GET_DESCRIPTOR;
  phost->Control.setup.b.wValue.w = value_idx;
  
  if ((value_idx & 0xff00) == USB_DESC_STRING)
  {
    phost->Control.setup.b.wIndex.w = 0x0409;
  }
  else
  {
    phost->Control.setup.b.wIndex.w = 0;
  }
  phost->Control.setup.b.wLength.w = length;           
  return USB_HOST_CtrlReq(pusbdev, phost, buff , length );     
}

/**
* @brief  USB_HOST_SetAddress
*         This command sets the address to the connected device
* @param  pusbdev: Selected device
* @param  DeviceAddress: Device address to assign
* @retval Status
*/
USB_HOST_Status USB_HOST_SetAddress(USB_OTG_CORE_HANDLE *pusbdev, 
                            USB_HOST *phost,
                            uint8_t DeviceAddress)
{
  phost->Control.setup.b.bmRequestType = USB_DIR_H2D | USB_REQUEST_RECIPIENT_DEVICE | \
    USB_REQUEST_TYPE_STANDARD;
  
  phost->Control.setup.b.bRequest = USB_REQUEST_SET_ADDRESS;
  
  phost->Control.setup.b.wValue.w = (uint16_t)DeviceAddress;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;
  
  return USB_HOST_CtrlReq(pusbdev, phost, 0 , 0 );
}

/**
* @brief  USB_HOST_SetCfg
*         The command sets the configuration value to the connected device
* @param  pusbdev: Selected device
* @param  cfg_idx: Configuration value
* @retval Status
*/
USB_HOST_Status USB_HOST_SetCfg(USB_OTG_CORE_HANDLE *pusbdev, 
                        USB_HOST *phost,
                        uint16_t cfg_idx)
{
  
  phost->Control.setup.b.bmRequestType = USB_DIR_H2D | USB_REQUEST_RECIPIENT_DEVICE |\
    USB_REQUEST_TYPE_STANDARD;
  phost->Control.setup.b.bRequest = USB_REQUEST_SET_CONFIGURATION;
  phost->Control.setup.b.wValue.w = cfg_idx;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;           
  
  return USB_HOST_CtrlReq(pusbdev, phost, 0 , 0 );      
}

/**
* @brief  USB_HOST_SetInterface
*         The command sets the Interface value to the connected device
* @param  pusbdev: Selected device
* @param  itf_idx: Interface value
* @retval Status
*/
USB_HOST_Status USB_HOST_SetInterface(USB_OTG_CORE_HANDLE *pusbdev, 
                        USB_HOST *phost,
                        uint8_t ept_num, uint8_t altSetting)
{
  phost->Control.setup.b.bmRequestType = USB_DIR_H2D | USB_REQUEST_RECIPIENT_INTERFACE | \
    USB_REQUEST_TYPE_STANDARD;
  
  phost->Control.setup.b.bRequest = USB_REQUEST_SET_INTERFACE;
  phost->Control.setup.b.wValue.w = altSetting;
  phost->Control.setup.b.wIndex.w = ept_num;
  phost->Control.setup.b.wLength.w = 0;           
  
  return USB_HOST_CtrlReq(pusbdev, phost, 0 , 0 );     
}

/**
  * @brief  USBH_SetFeature
  *         The command sets the device features (remote wakeup feature,..)
  * @param  pusbdev: Selected device
  * @param  itf_idx
  * @retval Status
*/
USB_HOST_Status USB_HOST_SetDeviceFeature(USB_OTG_CORE_HANDLE *pusbdev, 
                                  USB_HOST *phost,
                                  uint8_t FeatureSelector, uint16_t wIndex)
{
  phost->Control.setup.b.bmRequestType = USB_DIR_H2D | USB_REQUEST_RECIPIENT_DEVICE | \
    USB_REQUEST_TYPE_STANDARD;
  
  phost->Control.setup.b.bRequest = USB_REQUEST_SET_FEATURE;
  phost->Control.setup.b.wValue.w = FeatureSelector;
  phost->Control.setup.b.wIndex.w = wIndex;
  phost->Control.setup.b.wLength.w = 0;           
  
  return USB_HOST_CtrlReq(pusbdev, phost, 0 , 0 );     
}


/**
* @brief  USB_HOST_ClearDeviceFeature
*         The command sets the device features (remote wakeup feature,..)
* @param  pusbdev: Selected device
* @param  itf_idx
* @retval Status
*/
USB_HOST_Status USB_HOST_ClearDeviceFeature(USB_OTG_CORE_HANDLE *pusbdev, 
                                    USB_HOST *phost,
                                    uint8_t FeatureSelector, uint16_t wIndex)
{
  phost->Control.setup.b.bmRequestType = USB_DIR_H2D | USB_REQUEST_RECIPIENT_DEVICE | \
    USB_REQUEST_TYPE_STANDARD;
  
  phost->Control.setup.b.bRequest = USB_REQUEST_CLEAR_FEATURE;
  phost->Control.setup.b.wValue.w = FeatureSelector;
  phost->Control.setup.b.wIndex.w = wIndex;
  phost->Control.setup.b.wLength.w = 0;           
  
  return USB_HOST_CtrlReq(pusbdev, phost, 0 , 0 );     
}

/**
* @brief  USB_HOST_ClrFeature
*         This request is used to clear or disable a specific feature.

* @param  pusbdev: Selected device
* @param  ept_num: endpoint number 
* @param  hc_num: Host channel number 
* @retval Status
*/
USB_HOST_Status USB_HOST_ClrFeature(USB_OTG_CORE_HANDLE *pusbdev,
                            USB_HOST *phost,
                            uint8_t ept_num, 
                            uint8_t hc_num) 
{
  
  phost->Control.setup.b.bmRequestType = USB_DIR_H2D | 
                                         USB_REQUEST_RECIPIENT_ENDPOINT |
                                         USB_REQUEST_TYPE_STANDARD;
  
  phost->Control.setup.b.bRequest = USB_REQUEST_CLEAR_FEATURE;
  phost->Control.setup.b.wValue.w = USB_FEATURE_SELECTOR_ENDPOINT;
  phost->Control.setup.b.wIndex.w = ept_num;
  phost->Control.setup.b.wLength.w = 0;           
  
  if ((ept_num & USB_REQUEST_DIR_MASK ) == USB_DIR_D2H)
  { /* EP Type is IN */
    pusbdev->host.hc[hc_num].toggle_in = 0; 
  }
  else
  {/* EP Type is OUT */
    pusbdev->host.hc[hc_num].toggle_out = 0; 
  }
  
  return USB_HOST_CtrlReq(pusbdev, phost, 0 , 0 );   
}

/**
* @brief  USB_HOST_ParseDevDesc 
*         This function Parses the device descriptor
* @param  dev_desc: device_descriptor destination address 
* @param  buf: Buffer where the source descriptor is available
* @param  length: Length of the descriptor
* @retval None
*/
static void  USB_HOST_ParseDevDesc (USB_HOST_DevDesDesc_Type* dev_desc,
                                uint8_t *buf, 
                                uint16_t length)
{
  dev_desc->bLength            = *(uint8_t  *) (buf +  0);
  dev_desc->bDescriptorType    = *(uint8_t  *) (buf +  1);
  dev_desc->bcdUSB             = LE16 (buf +  2);
  dev_desc->bDeviceClass       = *(uint8_t  *) (buf +  4);
  dev_desc->bDeviceSubClass    = *(uint8_t  *) (buf +  5);
  dev_desc->bDeviceProtocol    = *(uint8_t  *) (buf +  6);
  dev_desc->bMaxPacketSize     = *(uint8_t  *) (buf +  7);
  
  if (length > 8)
  { /* For 1st time after device connection, Host may issue only 8 bytes for 
    Device Descriptor Length  */
    dev_desc->idVendor           = LE16 (buf +  8);
    dev_desc->idProduct          = LE16 (buf + 10);
    dev_desc->bcdDevice          = LE16 (buf + 12);
    dev_desc->iManufacturer      = *(uint8_t  *) (buf + 14);
    dev_desc->iProduct           = *(uint8_t  *) (buf + 15);
    dev_desc->iSerialNumber      = *(uint8_t  *) (buf + 16);
    dev_desc->bNumConfigurations = *(uint8_t  *) (buf + 17);
  }
}

/**
* @brief  USB_HOST_ParseCfgDesc 
*         This function Parses the configuration descriptor
* @param  cfg_desc: Configuration Descriptor address
* @param  itf_desc: Interface Descriptor address
* @param  ep_desc: Endpoint Descriptor address
* @param  buf: Buffer where the source descriptor is available
* @param  length: Length of the descriptor
* @retval None
*/
USB_HOST_Status USB_HOST_ParseCfgDesc (USB_HOST_CfgDesc_Type* cfg_desc,
                                USB_HOST_InterfaceDesc_Type* itf_desc,
                                USB_HOST_EpDesc_Type   ep_desc[][USBH_MAX_NUM_ENDPOINTS], 
                                uint8_t *buf, 
                                uint16_t length)
{  
  USB_HOST_InterfaceDesc_Type    *pif ;
  USB_HOST_InterfaceDesc_Type    temp_pif ;  
  USB_HOST_EpDesc_Type           *pep;  
  USBH_DescriptorHeader             *pdesc = (USBH_DescriptorHeader *)buf;
  uint16_t                      ptr;
  int8_t                        if_ix = 0;
  int8_t                        ep_ix = 0;  
  static uint16_t               prev_ep_size = 0;
  static uint8_t                prev_itf = 0;  
  USB_HOST_Status status;
  
  
  pdesc   = (USBH_DescriptorHeader *)buf;
  
  /* Parse configuration descriptor */
  cfg_desc->bLength             = *(uint8_t  *) (buf + 0);
  cfg_desc->bDescriptorType     = *(uint8_t  *) (buf + 1);
  cfg_desc->wTotalLength        = LE16 (buf + 2);
  cfg_desc->bNumInterfaces      = *(uint8_t  *) (buf + 4);
  cfg_desc->bConfigurationValue = *(uint8_t  *) (buf + 5);
  cfg_desc->iConfiguration      = *(uint8_t  *) (buf + 6);
  cfg_desc->bmAttributes        = *(uint8_t  *) (buf + 7);
  cfg_desc->bMaxPower           = *(uint8_t  *) (buf + 8);    
  
  
  if (length > USB_CONFIGURATION_DESC_SIZE)
  {
    ptr = USB_CFG_DESC_LEN;
    
    if ( cfg_desc->bNumInterfaces <= USBH_MAX_NUM_INTERFACES) 
    {
      pif = (USB_HOST_InterfaceDesc_Type *)0;
      while (ptr < cfg_desc->wTotalLength ) 
      {
        pdesc = USB_HOST_GetNextDesc((uint8_t *)pdesc, &ptr);
        if (pdesc->bDescriptorType   == USB_DESC_TYPE_INTERFACE) 
        {
          if_ix             = *(((uint8_t *)pdesc ) + 2);
          pif               = &itf_desc[if_ix];
          if((*((uint8_t *)pdesc + 3)) < 3)
          {
            USB_HOST_ParseInterfaceDesc (&temp_pif, (uint8_t *)pdesc);            
            ep_ix = 0;
            
            /* Parse Ep descriptors relative to the current interface */
            if(temp_pif.bNumEndpoints <= USBH_MAX_NUM_ENDPOINTS)
            {          
              while (ep_ix < temp_pif.bNumEndpoints) 
              {
                pdesc = USB_HOST_GetNextDesc((void* )pdesc, &ptr);
                if (pdesc->bDescriptorType   == USB_DESC_TYPE_ENDPOINT) 
                {  
                  pep               = &ep_desc[if_ix][ep_ix];
                  if(prev_itf != if_ix)
                  {
                    prev_itf = if_ix;
                    USB_HOST_ParseInterfaceDesc (pif, (uint8_t *)&temp_pif); 
                  }
                  else
                  {
                    if(prev_ep_size > LE16((uint8_t *)pdesc + 4))
                    {
                      break;
                    }
                    else
                    {
                      USB_HOST_ParseInterfaceDesc (pif, (uint8_t *)&temp_pif);    
                    }
                  }
                  USB_HOST_ParseEPDesc (pep, (uint8_t *)pdesc);
                  prev_ep_size = LE16((uint8_t *)pdesc + 4);
                  ep_ix++;
                }
              }
            }
            else /*num endpoints exceeded */
            {
              status = USB_HOST_NOT_SUPPORTED;
              return status;
            } 
          }
        }
      }
    }
    else /*num interfaces exceeded */
    {
      status = USB_HOST_NOT_SUPPORTED;
      return status;
    } 
    prev_ep_size = 0;
    prev_itf = 0; 
  }
  return USB_HOST_OK ;
}


/**
* @brief  USB_HOST_ParseInterfaceDesc 
*         This function Parses the interface descriptor
* @param  if_descriptor : Interface descriptor destination
* @param  buf: Buffer where the descriptor data is available
* @retval None
*/
static void  USB_HOST_ParseInterfaceDesc (USB_HOST_InterfaceDesc_Type *if_descriptor, 
                                      uint8_t *buf)
{
  if_descriptor->bLength            = *(uint8_t  *) (buf + 0);
  if_descriptor->bDescriptorType    = *(uint8_t  *) (buf + 1);
  if_descriptor->bInterfaceNumber   = *(uint8_t  *) (buf + 2);
  if_descriptor->bAlternateSetting  = *(uint8_t  *) (buf + 3);
  if_descriptor->bNumEndpoints      = *(uint8_t  *) (buf + 4);
  if_descriptor->bInterfaceClass    = *(uint8_t  *) (buf + 5);
  if_descriptor->bInterfaceSubClass = *(uint8_t  *) (buf + 6);
  if_descriptor->bInterfaceProtocol = *(uint8_t  *) (buf + 7);
  if_descriptor->iInterface         = *(uint8_t  *) (buf + 8);
}

/**
* @brief  USB_HOST_ParseEPDesc 
*         This function Parses the endpoint descriptor
* @param  ep_descriptor: Endpoint descriptor destination address
* @param  buf: Buffer where the parsed descriptor stored
* @retval None
*/
static void  USB_HOST_ParseEPDesc (USB_HOST_EpDesc_Type  *ep_descriptor, 
                               uint8_t *buf)
{
  
  ep_descriptor->bLength          = *(uint8_t  *) (buf + 0);
  ep_descriptor->bDescriptorType  = *(uint8_t  *) (buf + 1);
  ep_descriptor->bEndpointAddress = *(uint8_t  *) (buf + 2);
  ep_descriptor->bmAttributes     = *(uint8_t  *) (buf + 3);
  ep_descriptor->wMaxPacketSize   = LE16 (buf + 4);
  ep_descriptor->bInterval        = *(uint8_t  *) (buf + 6);
}

/**
* @brief  USB_HOST_ParseStringDesc 
*         This function Parses the string descriptor
* @param  psrc: Source pointer containing the descriptor data
* @param  pdest: Destination address pointer
* @param  length: Length of the descriptor
* @retval None
*/
static void USB_HOST_ParseStringDesc (uint8_t* psrc, 
                                  uint8_t* pdest, 
                                  uint16_t length)
{
  uint16_t strlength;
  uint16_t idx;
  
  /* The UNICODE string descriptor is not NULL-terminated. The string length is
  computed by subtracting two from the value of the first byte of the descriptor.
  */
  
  /* Check which is lower size, the Size of string or the length of bytes read 
  from the device */
  
  if ( psrc[1] == USB_DESC_TYPE_STRING)
  { /* Make sure the Descriptor is String Type */
    
    /* psrc[0] contains Size of Descriptor, subtract 2 to get the length of string */      
    strlength = ( ( (psrc[0]-2) <= length) ? (psrc[0]-2) :length); 
    psrc += 2; /* Adjust the offset ignoring the String Len and Descriptor ept_type */
    
    for (idx = 0; idx < strlength; idx+=2 )
    {/* Copy Only the string and ignore the UNICODE ID, hence add the src */
      *pdest =  psrc[idx];
      pdest++;
    }  
    *pdest = 0; /* mark end of string */  
  }
}

/**
* @brief  USB_HOST_GetNextDesc 
*         This function return the next descriptor header
* @param  buf: Buffer where the cfg descriptor is available
* @param  ptr: data pointer inside the cfg descriptor
* @retval next header
*/
USBH_DescriptorHeader  *USB_HOST_GetNextDesc (uint8_t   *pbuf, uint16_t  *ptr)
{
  USBH_DescriptorHeader  *pnext;
 
  *ptr += ((USBH_DescriptorHeader *)pbuf)->bLength;  
  pnext = (USBH_DescriptorHeader *)((uint8_t *)pbuf + \
         ((USBH_DescriptorHeader *)pbuf)->bLength);
 
  return(pnext);
}

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/ 





