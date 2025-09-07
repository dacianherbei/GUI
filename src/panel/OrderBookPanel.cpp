//
// Created by dh on 03/09/2025.
//

#include "OrderBookPanel.h"

#include <algorithm>

namespace gui {
namespace panel {

OrderBookPanel::OrderBookPanel()
    : m_currencyPair("BTC/USD")
    , m_lastPrice(50000.0)
    , m_volume24h(1234.56)
    , m_spreadPercentage(0.02)
    , m_maxDisplayLevels(20)
    , m_showMarketDepth(true)
    , m_autoUpdate(true)
{
}

OrderBookPanel::~OrderBookPanel()
{
    Shutdown();
}

bool OrderBookPanel::Initialize()
{
    // Generate some sample data for testing
    GenerateSampleData();
    return true;
}

void OrderBookPanel::Render(bool* p_open)
{
    if (ImGui::Begin("Order Book", p_open))
    {
        RenderMarketInfo();
        ImGui::Separator();
        RenderOrderBookTable();
    }
    ImGui::End();
}

void OrderBookPanel::RenderMarketInfo()
{
    ImGui::Text("Market: %s", m_currencyPair.c_str());
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Last: $%.2f", m_lastPrice);

    ImGui::Text("24h Volume: %.2f", m_volume24h);
    ImGui::SameLine();
    ImGui::Text("Spread: %.3f%%", m_spreadPercentage);

    // Controls
    ImGui::Checkbox("Auto Update", &m_autoUpdate);
    ImGui::SameLine();
    ImGui::Checkbox("Market Depth", &m_showMarketDepth);

    ImGui::SetNextItemWidth(100.0f);
    ImGui::InputInt("Max Levels", &m_maxDisplayLevels);
    m_maxDisplayLevels = std::max(5, std::min(50, m_maxDisplayLevels));
}

void OrderBookPanel::RenderOrderBookTable()
{
    if (ImGui::BeginTable("OrderBookTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        // Sort orders by price
        auto sortedSellOrders = m_sellOrders;
        auto sortedBuyOrders = m_buyOrders;

        std::sort(sortedSellOrders.begin(), sortedSellOrders.end(),
                 [](const OrderBookEntry& a, const OrderBookEntry& b) { return a.price > b.price; });
        std::sort(sortedBuyOrders.begin(), sortedBuyOrders.end(),
                 [](const OrderBookEntry& a, const OrderBookEntry& b) { return a.price > b.price; });

        // Display sell orders (asks) - higher prices first
        int displayedSells = std::min(m_maxDisplayLevels / 2, (int)sortedSellOrders.size());
        double sellCumulative = 0.0;

        for (int i = 0; i < displayedSells; i++)
        {
            const auto& order = sortedSellOrders[i];
            sellCumulative += order.size;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%.2f", order.price);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.4f", order.size);

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.4f", sellCumulative);
        }

        // Spread row
        if (!sortedSellOrders.empty() && !sortedBuyOrders.empty())
        {
            double spread = sortedSellOrders.back().price - sortedBuyOrders.front().price;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "---");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Spread: %.2f", spread);
            ImGui::TableSetColumnIndex(2);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "---");
        }

        // Display buy orders (bids) - higher prices first
        int displayedBuys = std::min(m_maxDisplayLevels / 2, (int)sortedBuyOrders.size());
        double buyCumulative = 0.0;

        for (int i = 0; i < displayedBuys; i++)
        {
            const auto& order = sortedBuyOrders[i];
            buyCumulative += order.size;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%.2f", order.price);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.4f", order.size);

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.4f", buyCumulative);
        }

        ImGui::EndTable();
    }
}

void OrderBookPanel::SetCurrencyPair(const std::string& pair)
{
    m_currencyPair = pair;
    GenerateSampleData(); // Refresh data for new pair
}

void OrderBookPanel::UpdateOrderBook(const std::vector<OrderBookEntry>& buyOrders,
                                    const std::vector<OrderBookEntry>& sellOrders)
{
    m_buyOrders = buyOrders;
    m_sellOrders = sellOrders;

    // Calculate spread
    if (!buyOrders.empty() && !sellOrders.empty())
    {
        double bestBid = std::max_element(buyOrders.begin(), buyOrders.end(),
                                         [](const OrderBookEntry& a, const OrderBookEntry& b) {
                                             return a.price < b.price;
                                         })->price;
        double bestAsk = std::min_element(sellOrders.begin(), sellOrders.end(),
                                         [](const OrderBookEntry& a, const OrderBookEntry& b) {
                                             return a.price < b.price;
                                         })->price;
        m_spreadPercentage = ((bestAsk - bestBid) / bestBid) * 100.0;
    }
}

void OrderBookPanel::GenerateSampleData()
{
    m_buyOrders.clear();
    m_sellOrders.clear();

    // Generate sample buy orders (bids)
    double basePrice = m_lastPrice;
    for (int i = 0; i < 15; i++)
    {
        double price = basePrice - (i + 1) * 5.0;
        double size = 0.1 + (i * 0.05);
        m_buyOrders.emplace_back(price, size, true);
    }

    // Generate sample sell orders (asks)
    for (int i = 0; i < 15; i++)
    {
        double price = basePrice + (i + 1) * 5.0;
        double size = 0.1 + (i * 0.03);
        m_sellOrders.emplace_back(price, size, false);
    }
}

void OrderBookPanel::Shutdown()
{
    // Cleanup if needed
}
} // namespace panel
} // namespace gui