//
// Created by dh on 03/09/2025.
//

#include "MainWindow.h"

namespace gui
{
    void MainWindow::RenderStatusBar()
    {
        if (ImGui::BeginViewportSideBar("##StatusBar", NULL, ImGuiDir_Down,
                                        ImGui::GetFrameHeight(), ImGuiWindowFlags_NoScrollbar))
        {
            if (m_exchangeState.isConnected)
            {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "Connected to %s",
                                   m_exchangeState.exchangeName.c_str());
            }
            else
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Disconnected");
            }

            ImGui::SameLine();
            ImGui::Text(" | Status: %s", m_exchangeState.connectionStatus.c_str());

            // Add more status information
            ImGui::SameLine();
            ImGui::Text(" | Time: %.1f", ImGui::GetTime());

            ImGui::End();
        }
    }

    void MainWindow::RenderExchangeList()
    {
        if (!ImGui::Begin("Exchange List"))
        {
            ImGui::End();
            return;
        }

        // Filter input
        ImGui::InputText("Filter", m_uiState.symbolFilter, sizeof(m_uiState.symbolFilter));
        ImGui::Separator();

        // Symbol list
        for (int i = 0; i < m_exchangeState.availableSymbols.size(); i++)
        {
            const auto& symbol = m_exchangeState.availableSymbols[i];

            // Apply filter
            if (strlen(m_uiState.symbolFilter) > 0 &&
                symbol.find(m_uiState.symbolFilter) == std::string::npos)
                continue;

            if (ImGui::Selectable(symbol.c_str(), m_uiState.selectedSymbolIndex == i))
            {
                m_uiState.selectedSymbolIndex = i;
            }
        }

        ImGui::End();
    }

    void MainWindow::RenderOrderBook()
    {
        if (!m_uiState.showOrderBook || !ImGui::Begin("Order Book", &m_uiState.showOrderBook))
        {
            if (m_uiState.showOrderBook) ImGui::End();
            return;
        }

        if (m_uiState.selectedSymbolIndex >= 0 &&
            m_uiState.selectedSymbolIndex < m_exchangeState.availableSymbols.size())
        {
            const auto& symbol = m_exchangeState.availableSymbols[m_uiState.selectedSymbolIndex];
            ImGui::Text("Order Book for %s", symbol.c_str());
            ImGui::Separator();

            // Create table for order book
            if (ImGui::BeginTable("OrderBookTable", 3, ImGuiTableFlags_Borders))
            {
                ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Side", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableHeadersRow();

                // Add your order book data here
                for (int i = 0; i < 10; i++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%.2f", 100.0f + i);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.1f", 10.0f + i);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextColored(i % 2 ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1),
                                       i % 2 ? "BUY" : "SELL");
                }

                ImGui::EndTable();
            }
        }
        else
        {
            ImGui::Text("Select a symbol to view order book");
        }

        ImGui::End();
    }

    void MainWindow::RenderTradingPanel()
    {
        if (!m_uiState.showTradingPanel || !ImGui::Begin("Trading Panel", &m_uiState.showTradingPanel))
        {
            if (m_uiState.showTradingPanel) ImGui::End();
            return;
        }

        static float orderPrice = 100.0f;
        static float orderSize = 1.0f;
        static int orderType = 0; // 0=Market, 1=Limit
        static int orderSide = 0; // 0=Buy, 1=Sell

        ImGui::Text("Place Order");
        ImGui::Separator();

        ImGui::Combo("Type", &orderType, "Market\0Limit\0");
        ImGui::Combo("Side", &orderSide, "Buy\0Sell\0");

        if (orderType == 1) // Limit order
        {
            ImGui::InputFloat("Price", &orderPrice, 0.01f, 1.0f, "%.2f");
        }

        ImGui::InputFloat("Size", &orderSize, 0.1f, 1.0f, "%.1f");

        ImGui::Separator();

        ImVec4 buttonColor = orderSide == 0 ? ImVec4(0, 0.7f, 0, 1) : ImVec4(0.7f, 0, 0, 1);
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);

        if (ImGui::Button(orderSide == 0 ? "BUY" : "SELL", ImVec2(-1, 0)))
        {
            // Implement order placement logic
            printf("Placing %s order: Type=%s, Price=%.2f, Size=%.1f\n",
                   orderSide == 0 ? "BUY" : "SELL",
                   orderType == 0 ? "Market" : "Limit",
                   orderPrice, orderSize);
        }

        ImGui::PopStyleColor();

        ImGui::End();
    }
} // gui
