#pragma once

#include "imgui.h"

#include <string>
#include <vector>

namespace gui {
namespace panel {

class OrderBookPanel
{
public:
    struct OrderBookEntry {
        double price;
        double size;
        bool isBuy; // true for buy orders, false for sell orders

        OrderBookEntry(double p, double s, bool buy) : price(p), size(s), isBuy(buy) {}
    };

    OrderBookPanel();
    ~OrderBookPanel();

    bool Initialize();
    void Render(bool* p_open = nullptr);
    void Shutdown();

    // Data management
    void SetCurrencyPair(const std::string& pair);
    void UpdateOrderBook(const std::vector<OrderBookEntry>& buyOrders,
                        const std::vector<OrderBookEntry>& sellOrders);
    void SetLastPrice(double price) { m_lastPrice = price; }
    void Set24hVolume(double volume) { m_volume24h = volume; }

private:
    void RenderOrderBookTable();
    void RenderMarketInfo();
    void GenerateSampleData(); // For testing

    std::string m_currencyPair;
    std::vector<OrderBookEntry> m_buyOrders;
    std::vector<OrderBookEntry> m_sellOrders;

    // Market data
    double m_lastPrice;
    double m_volume24h;
    double m_spreadPercentage;

    // UI settings
    int m_maxDisplayLevels;
    bool m_showMarketDepth;
    bool m_autoUpdate;
};

} // namespace panel
} // namespace gui
