#include "config.hpp"
#include "event.hpp"

void EventManager::update(float delta)
{
	if (const auto player = RE::PlayerCharacter::GetSingleton(); player)
	{
		if (allow_rotate && (std::abs(mouse_delta_x) > 0 || std::abs(gamepad_delta_x) > 0))
		{
			if (auto root = player->Get3D(false))
			{
				int dir = (mouse_delta_x > 0 || gamepad_delta_x > 0) ? -1 : 1;
				float delta_x = std::abs(mouse_delta_x) > 0.0f ? static_cast<float>(mouse_delta_x) : static_cast<float>(gamepad_delta_x);
				angle.z += dir * delta * std::lerp(config::min_rotate_speed, config::max_rotate_speed, std::abs(delta_x) / 360.f);

				root->local.rotate.SetEulerAnglesXYZ(angle);

				RE::NiUpdateData data;
				root->UpdateWorldData(&data);
			}
		}

		if (allow_pan && (std::abs(mouse_delta_x) > 0 || std::abs(mouse_delta_y) > 0 || std::abs(gamepad_delta_x) > 0 || std::abs(gamepad_delta_y) > 0))
		{
			if (auto camera = RE::RaceSexCamera::GetSingleton())
			{
				camera->pos.x += (mouse_delta_x + gamepad_delta_x) * delta * config::pan_speed;
				camera->pos.y += (mouse_delta_y + gamepad_delta_y) * delta * config::pan_speed;

				root->local.translate = camera->pos;
				
				RE::NiUpdateData data;
				root->UpdateWorldData(&data);
			}
		}
	}
}

RE::BSEventNotifyControl EventManager::ProcessEvent(RE::InputEvent* const* event, [[maybe_unused]] RE::BSTEventSource<RE::InputEvent*>* event_source)
{
	if (!event)
		return RE::BSEventNotifyControl::kContinue;

	if (const auto ui = RE::UI::GetSingleton(); ui && ui->IsMenuOpen(RE::RaceSexMenu::MENU_NAME))
	{
		for (auto input_event = *event; input_event; input_event = input_event->next)
		{
			switch (input_event->GetEventType())
			{
				case RE::INPUT_EVENT_TYPE::kButton:
				{
					auto button_event = input_event->AsButtonEvent();
					if (!button_event)
						continue;

					switch (input_event->GetDevice())
					{
						case RE::INPUT_DEVICE::kKeyboard:
							if (const auto key = button_event->GetIDCode(); key == config::rotate_key_code)
								allow_rotate = button_event->IsHeld();
							break;
						case RE::INPUT_DEVICE::kMouse:
							if (const auto mouseButton = button_event->GetIDCode(); mouseButton == config::rotate_mouse_button)
								allow_rotate = button_event->IsHeld();
							else if (mouseButton == config::pan_mouse_button)
								allow_pan = button_event->IsHeld();
							break;
						case RE::INPUT_DEVICE::kGamepad:
							if (const auto gamepadButton = button_event->GetIDCode(); gamepadButton == config::pan_gamepad_button)
								allow_pan = button_event->IsHeld();
							break;
					}
					break;
				}
				case RE::INPUT_EVENT_TYPE::kMouseMove:
				{
					auto mouse_event = reinterpret_cast<RE::MouseMoveEvent*>(input_event->AsIDEvent());
					mouse_delta_x = mouse_event->mouseInputX;
					mouse_delta_y = mouse_event->mouseInputY;
					break;
				}
				case RE::INPUT_EVENT_TYPE::kThumbstick:
				{
					auto thumbstick_event = reinterpret_cast<RE::ThumbstickEvent::kRightThumbstick*>(input_event->AsIDEvent());
					gamepad_delta_x = static_cast<int32_t>(thumbstick_event->xValue * 10);
					gamepad_delta_y = static_cast<int32_t>(thumbstick_event->yValue * 10);
					break;
				}
			}
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}

RE::BSEventNotifyControl EventManager::ProcessEvent(const RE::MenuOpenCloseEvent* event, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>* event_source)
{
	if (event && event->menuName == RE::RaceSexMenu::MENU_NAME)
	{
		if (event->opening) 
		{
			auto player = RE::PlayerCharacter::GetSingleton();
			if (auto root = player->Get3D(false))
				root->local.rotate.ToEulerAnglesXYZ(angle);
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}