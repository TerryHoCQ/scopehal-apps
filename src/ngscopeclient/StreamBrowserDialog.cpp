/***********************************************************************************************************************
*                                                                                                                      *
* ngscopeclient                                                                                                        *
*                                                                                                                      *
* Copyright (c) 2012-2024 Andrew D. Zonenberg                                                                          *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

/**
	@file
	@author Andrew D. Zonenberg
	@brief Implementation of StreamBrowserDialog
 */

#include "ngscopeclient.h"
#include "StreamBrowserDialog.h"
#include "MainWindow.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

StreamBrowserDialog::StreamBrowserDialog(Session& session, MainWindow* parent)
	: Dialog("Stream Browser", "Stream Browser", ImVec2(300, 400))
	, m_session(session)
	, m_parent(parent)
{

}

StreamBrowserDialog::~StreamBrowserDialog()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering

/**
	@brief Renders the dialog and handles UI events

	@return		True if we should continue showing the dialog
				False if it's been closed
 */
bool StreamBrowserDialog::DoRender()
{
	//Add all instruments
	auto insts = m_session.GetInstruments();
	for(auto inst : insts)
	{
		if(ImGui::TreeNodeEx(inst->m_nickname.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for(size_t i=0; i<inst->GetChannelCount(); i++)
			{
				auto chan = inst->GetChannel(i);

				bool open = ImGui::TreeNodeEx(chan->GetDisplayName().c_str(), ImGuiTreeNodeFlags_DefaultOpen);

				//Single stream: drag the stream not the channel
				bool singleStream = chan->GetStreamCount() == 1;
				if(singleStream)
				{
					StreamDescriptor s(chan, 0);
					if(ImGui::BeginDragDropSource())
					{
						if(s.GetType() == Stream::STREAM_TYPE_ANALOG_SCALAR)
							ImGui::SetDragDropPayload("Scalar", &s, sizeof(s));
						else
							ImGui::SetDragDropPayload("Stream", &s, sizeof(s));

						ImGui::TextUnformatted(s.GetName().c_str());
						ImGui::EndDragDropSource();
					}
					else
						DoItemHelp();
				}

				//Drag source for the channel itself (if we have zero or >1 streams)
				else if(ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("Channel", &chan, sizeof(chan));

					ImGui::TextUnformatted(chan->GetDisplayName().c_str());
					ImGui::EndDragDropSource();
				}

				if(open && !singleStream)
				{
					for(size_t j=0; j<chan->GetStreamCount(); j++)
					{
						ImGui::Selectable(chan->GetStreamName(j).c_str());

						StreamDescriptor s(chan, j);
						if(ImGui::BeginDragDropSource())
						{
							if(s.GetType() == Stream::STREAM_TYPE_ANALOG_SCALAR)
								ImGui::SetDragDropPayload("Scalar", &s, sizeof(s));
							else
								ImGui::SetDragDropPayload("Stream", &s, sizeof(s));

							ImGui::TextUnformatted(s.GetName().c_str());
							ImGui::EndDragDropSource();
						}
						else
							DoItemHelp();
					}
				}

				if(open)
					ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}

	//Add all filters
	if(ImGui::TreeNodeEx("Filters", ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto filters = Filter::GetAllInstances();
		for(auto f : filters)
		{
			bool open = ImGui::TreeNodeEx(f->GetDisplayName().c_str(), ImGuiTreeNodeFlags_DefaultOpen);

			//Single stream: drag the stream not the channel
			bool singleStream = f->GetStreamCount() == 1;
			if(singleStream)
			{
				StreamDescriptor s(f, 0);
				if(ImGui::BeginDragDropSource())
				{
					if(s.GetType() == Stream::STREAM_TYPE_ANALOG_SCALAR)
						ImGui::SetDragDropPayload("Scalar", &s, sizeof(s));
					else
						ImGui::SetDragDropPayload("Stream", &s, sizeof(s));

					ImGui::TextUnformatted(s.GetName().c_str());
					ImGui::EndDragDropSource();
				}
				else
					DoItemHelp();
			}

			//Drag source for the channel itself (if we have zero or >1 streams)
			else if(ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("Channel", &f, sizeof(f));

				ImGui::TextUnformatted(f->GetDisplayName().c_str());
				ImGui::EndDragDropSource();
			}

			if(open && !singleStream)
			{
				for(size_t j=0; j<f->GetStreamCount(); j++)
				{
					ImGui::Selectable(f->GetStreamName(j).c_str());

					StreamDescriptor s(f, j);
					if(ImGui::BeginDragDropSource())
					{
						if(s.GetType() == Stream::STREAM_TYPE_ANALOG_SCALAR)
							ImGui::SetDragDropPayload("Scalar", &s, sizeof(s));
						else
							ImGui::SetDragDropPayload("Stream", &s, sizeof(s));

						ImGui::TextUnformatted(s.GetName().c_str());
						ImGui::EndDragDropSource();
					}
					else
						DoItemHelp();
				}
			}

			if(open)
				ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	return true;
}

void StreamBrowserDialog::DoItemHelp()
{
	if(ImGui::IsItemHovered())
		m_parent->AddStatusHelp("mouse_lmb_drag", "Add to filter graph or plot");
}
