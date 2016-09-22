// Author: Jacques Newman
#include <Stdafx.h>

GUIWindow::WindowContainer * GUIWindow::s_windows = nullptr;
GUIWindow::GUIWindow(const char* name, DrawFunction function, 
                      bool StartActive, ImGuiWindowFlags flags)
                      : m_active(StartActive), m_flags(flags),
                       m_name(name), m_draw_function(function)
{
  if (s_windows == nullptr)
  {
    s_windows = new WindowContainer;
  }
  (*s_windows)[name] = this;
}

GUIWindow::~GUIWindow()
{
  s_windows->erase(m_name);
}

const char* GUIWindow::GetName()
{
  return m_name;
}

void GUIWindow::Draw()
{
  if (m_active == false) return;
  ImGui::Begin(m_name, &m_active, m_flags);
  if (m_draw_function)
  {
    m_draw_function(this);
  }
  ImGui::End();
}

void GUIWindow::DrawWindows()
{
  for (auto window_ptr : *s_windows)
  {
    window_ptr.second->Draw();
  }
}

void GUIWindow::SetActive(const std::string& name, bool active)
{
  if (s_windows->find(name) != s_windows->end())
  {
    (*s_windows)[name]->m_active = active;
  }
}
