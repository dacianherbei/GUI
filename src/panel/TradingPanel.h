#pragma once

#include "imgui.h"
#include <string>
#include <functional>

namespace gui {
namespace panel {

class TradingPanel
{
public:
    enum class OrderType
    {
        Market,
        Limit,
        Stop,
        StopLimit
    };

    enum class OrderSide
    {
        Buy,
        Sell
    };

    struct OrderRequest
    {
        OrderType type;
        OrderSide side;
        double price;
        double amount;
        double stopPrice; // For stop orders
        std::string currencyPair;
    };

    // Callback for when an order is placed
    using OrderCallback = std::function<void(const OrderRequest&)>;

    TradingPanel();
    ~TradingPanel();

    bool Initialize();
    void Render(bool* p_open = nullptr);
    void Shutdown();

    // Configuration
    void SetCurrencyPair(const std::string& pair);
    void SetOrderCallback(OrderCallback callback) { m_orderCallback = callback; }

    // Account balance
    void SetBalance(double baseBalance, double quoteBalance);
    void SetCurrentPrice(double price) { m_currentPrice = price; }

private:
    void RenderOrderTypeSelection();
    void RenderOrderSideSelection();
    void RenderPriceAmountInputs();
    void RenderOrderButton();
    void RenderAccountBalance();
    void CalculateOrderTotal();

    std::string m_currencyPair;
    std::string m_baseCurrency;
    std::string m_quoteCurrency;

    // Order parameters
    OrderType m_orderType;
    OrderSide m_orderSide;
    float m_price;
    float m_amount;
    float m_stopPrice;
    float m_orderTotal;

    // Account information
    double m_baseBalance;
    double m_quoteBalance;
    double m_currentPrice;

    // Callback
    OrderCallback m_orderCallback;

    // UI state
    bool m_confirmOrder;
};

} // namespace panel
} // namespace gui

