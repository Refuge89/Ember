/*
 * Copyright (c) 2016 Ember
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "InQueue.h"
#include "Authentication.h"
#include "../EventTypes.h"
#include "../Locator.h"
#include "../ClientHandler.h"
#include "../ClientConnection.h"
#include "../RealmQueue.h"
#include <logger/Logging.h>
#include <game_protocol/server/SMSG_AUTH_RESPONSE.h>

namespace ember { namespace queue {

void handle_queue_update(ClientContext* ctx, const QueuePosition* event) {
	protocol::SMSG_AUTH_RESPONSE packet;
	packet.result = protocol::ResultCode::AUTH_WAIT_QUEUE;
	packet.queue_position = event->position;
	ctx->connection->send(packet);
}

void handle_queue_success(ClientContext* ctx, const QueueSuccess* event) {
	authentication::auth_success(ctx, event->packet);
}

void enter(ClientContext* ctx) {
	LOG_DEBUG_GLOB << ctx->account_name << " added to queue" << LOG_ASYNC;
}

void handle_event(ClientContext* ctx, const Event* event) {
	switch(event->type) {
		case EventType::QUEUE_UPDATE_POSITION:
			handle_queue_update(ctx, static_cast<const QueuePosition*>(event));
			break;
		case EventType::QUEUE_SUCCESS:
			handle_queue_success(ctx, static_cast<const QueueSuccess*>(event));
			break;
	}
}

void handle_packet(ClientContext* ctx) {
	// don't care
}

void exit(ClientContext* ctx) {
	LOG_DEBUG_GLOB << ctx->account_name << " removed from queue" << LOG_ASYNC;

	if(ctx->state == ClientState::SESSION_CLOSED) {
		Locator::queue()->dequeue(ctx->handler->uuid());
	}
}

}} // queue, ember