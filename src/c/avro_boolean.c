/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/
#include "avro_private.h"

struct avro_boolean_value
{
  int value;
  int value_set;
  struct avro_value base_value;
};

static void
avro_boolean_print (struct avro_value *value, FILE * fp)
{
  struct avro_boolean_value *self =
    container_of (value, struct avro_boolean_value, base_value);
  avro_value_indent (value, fp);
  fprintf (fp, "boolean");
  if (self->value_set)
    {
      fprintf (fp, " value=%d", self->value);
    }
  fprintf (fp, "\n");
}

static avro_status_t
avro_boolean_read_skip (struct avro_value *value,
			struct avro_channel *channel, int skip)
{
  char b;
  avro_status_t status;
  struct avro_io *io;
  struct avro_boolean_value *self =
    container_of (value, struct avro_boolean_value, base_value);
  if (!channel)
    {
      return AVRO_FAILURE;
    }
  io = channel->io;
  if (!io)
    {
      return AVRO_FAILURE;
    }
  if (skip)
    {
      status = io->skip (io, 1);
    }
  else
    {
      status = io->read (io, &b, 1);
    }
  if (status != AVRO_OK)
    {
      return status;
    }
  self->value_set = 0;
  if (!skip)
    {
      self->value = b;
      self->value_set = 1;
    }
  return AVRO_OK;
}

static avro_status_t
avro_boolean_read (struct avro_value *value, struct avro_channel *channel)
{
  return avro_boolean_read_skip (value, channel, 0);
}

static avro_status_t
avro_boolean_skip (struct avro_value *value, struct avro_channel *channel)
{
  return avro_boolean_read_skip (value, channel, 1);
}

static avro_status_t
avro_boolean_write (struct avro_value *value, struct avro_channel *channel)
{
  char b;
  struct avro_boolean_value *self =
    container_of (value, struct avro_boolean_value, base_value);
  struct avro_io *io;
  if (!channel)
    {
      return AVRO_FAILURE;
    }
  io = channel->io;
  if (!io)
    {
      return AVRO_FAILURE;
    }
  b = self->value;
  return io->write (io, &b, 1);
}

static struct avro_value *
avro_boolean_create (struct avro_value_ctx *ctx, struct avro_value *parent,
		     apr_pool_t * pool, const JSON_value * json)
{
  struct avro_boolean_value *self =
    apr_palloc (pool, sizeof (struct avro_boolean_value));
  if (!self)
    {
      return NULL;
    }
  self->base_value.type = AVRO_BOOLEAN;
  self->base_value.pool = pool;
  self->base_value.parent = parent;
  self->base_value.schema = json;
  self->value_set = 0;
  return &self->base_value;
}

const struct avro_value_info avro_boolean_info = {
  .name = L"boolean",
  .type = AVRO_BOOLEAN,
  .private = 0,
  .create = avro_boolean_create,
  .formats = {{
	       .read_data = avro_boolean_read,
	       .skip_data = avro_boolean_skip,
	       .write_data = avro_boolean_write},
	      {
	       /* TODO: import/export */
	       .read_data = avro_boolean_read,
	       .skip_data = avro_boolean_skip,
	       .write_data = avro_boolean_write}},
  .print_info = avro_boolean_print
};
