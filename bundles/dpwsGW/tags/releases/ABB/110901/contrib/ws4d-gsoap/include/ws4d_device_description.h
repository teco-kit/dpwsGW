/* WS4D-gSOAP - Implementation of the Devices Profile for Web Services
 * (DPWS) on top of gSOAP
 * Copyright (C) 2007 University of Rostock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef WS4D_DEVICE_DESCRIPTION_H_
#define WS4D_DEVICE_DESCRIPTION_H_

/**
 * @addtogroup APIDescriptionDevice Device description
 * @ingroup APIHosting
 *
 * @{
 */

struct ws4d_thisDevice
{
  int __sizeFriendlyName;
  struct ws4d_locstring *FriendlyName;
  const char *FirmwareVersion;
  const char *SerialNumber;
  char *__any;
  char *__anyAttribute;
};

#define ws4d_device_FriendlyName_var(var) \
  struct ws4d_locstring var[]

#define ws4d_device_set_FriendlyName(_device, strings, count) \
  _device->FriendlyName = (struct ws4d_locstring *) strings;\
  _device->__sizeFriendlyName = count

#define ws4d_device_get_FriendlyName(_device, strings, count) \
  strings = _device->FriendlyName;\
  count = _device->__sizeFriendlyName = count;

#define ws4d_device_FirmwareVersion_var(var) \
  char *var

#define ws4d_device_set_FirmwareVersion(_device, number) \
  _device->FirmwareVersion = (char *) number;

/* TODO: define get macro */

#define ws4d_device_SerialNumber_var(var) \
  char *var

#define ws4d_device_set_SerialNumber(_device, number) \
  _device->SerialNumber = (char *) number;

/* TODO: define get macro */

#define ws4d_device_any_var(var) \
  char *var

#define ws4d_device_set_any(_device, string) \
  _device->any = (char *) string;

/* TODO: define get macro */

/** @} */

/**
 * @addtogroup APIDescriptionModel Model description
 * @ingroup APIHosting
 *
 * @{
 */

struct ws4d_thisModel
{
  int __sizeManufacturer;
  struct ws4d_locstring *Manufacturer;
  char *ManufacturerUrl;
  int __sizeModelName;
  struct ws4d_locstring *ModelName;
  char *ModelNumber;
  char *ModelUrl;
  char *PresentationUrl;
  char *__any;
  char *__anyAttribute;
};

#define ws4d_model_Manufacturer_var(var) \
  struct ws4d_locstring var[]

#define ws4d_model_set_Manufacturer(_model, strings, count) \
  _model->Manufacturer = (struct ws4d_locstring *) strings; \
  _model->__sizeManufacturer = count

/* TODO: define get macro */

#define ws4d_model_ManufacturerUrl_var(var) \
  char *var

#define ws4d_model_set_ManufacturerUrl(_model, url) \
  _model->ManufacturerUrl = (char *) url;

/* TODO: define get macro */

#define ws4d_model_Name_var(var) \
  struct ws4d_locstring var[]

#define ws4d_model_set_Name(_model, strings, count) \
  _model->ModelName = (struct ws4d_locstring *) strings; \
  _model->__sizeModelName = count

/* TODO: define get macro */

#define ws4d_model_Number_var(var) \
  char *var

#define ws4d_model_set_Number(_model, number) \
  _model->ModelNumber = (char *) number;

/* TODO: define get macro */

#define ws4d_model_Url_var(var) \
  char *var

#define ws4d_model_set_Url(_model, url) \
  _model->ModelUrl = (char *) url;

/* TODO: define get macro */

#define ws4d_model_PresentationUrl_var(var) \
  char *var

#define ws4d_model_set_PresentationUrl(_model, url) \
  _model->PresentationUrl = (char *) url;

/* TODO: define get macro */

#define ws4d_model_any_var(var) \
  char *var

#define ws4d_model_set_any(_model, string) \
  _model->__any = (char *) string;

/* TODO: define get macro */

/** @} */

#endif /*WS4D_DEVICE_DESCRIPTION_H_ */
