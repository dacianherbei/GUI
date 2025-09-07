//
// Created by dh on 03/09/2025.
//

#include "TradingPanel.h"

namespace gui {
namespace panel {
#include "TradingPanel.h"
#include <stdio.h>

TradingPanel::TradingPanel()
    : m_currencyPair("BTC/USD")
    , m_baseCurrency("BTC")
    , m_quoteCurrency("USD")
    , m_orderType(OrderType::Limit)
    , m_orderSide(OrderSide::Buy)
    , m_price(50000.0f)
    , m_amount(0.1f)
    , m_stopPrice(49000.0f)
    , m_orderTotal(5000.0f)
    , m_baseBalance(0.5)
    , m_quoteBalance(10000.0)
    , m_currentPrice(50000.0)
    , m_confirmOrder(false)
{
}

TradingPanel::~TradingPanel()
{
    Shutdown();
}

bool TradingPanel::Initialize()
{
    CalculateOrderTotal();
    return true;
}

void TradingPanel::Render(bool* p_open)
{
    if (ImGui::Begin("Trading Panel", p_open))
    {
        ImGui::Text("Trade %s", m_currencyPair.c_str());
        ImGui::Separator();

        RenderOrderTypeSelection();
        ImGui::Spacing();

        RenderOrderSideSelection();
        ImGui::Spacing();

        RenderPriceAmountInputs();
        ImGui::Spacing();

        RenderOrderButton();
        ImGui::Spacing();

        ImGui::Separator();
        RenderAccountBalance();
    }
    ImGui::End();
}

void TradingPanel::RenderOrderTypeSelection()
{
    ImGui::Text("Order Type:");

    const char* orderTypes[] = { "Market", "Limit", "Stop", "Stop Limit" };
    int currentType = static_cast<int>(m_orderType);

    if (ImGui::Combo("##OrderType", &currentType, orderTypes, IM_ARRAYSIZE(orderTypes)))
    {
        m_orderType = static_cast<OrderType>(currentType);
        CalculateOrderTotal();
    }
}

void TradingPanel::RenderOrderSideSelection()
{
    ImGui::Text("Side:");

    // Buy/Sell radio buttons with colors
    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
    bool isBuy = (m_orderSide == OrderSide::Buy);
    if (ImGui::RadioButton("Buy", isBuy))
    {
        m_orderSide = OrderSide::Buy;
        CalculateOrderTotal();
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    bool isSell = (m_orderSide == OrderSide::Sell);
    if (ImGui::RadioButton("Sell", isSell))
    {
        m_orderSide = OrderSide::Sell;
        CalculateOrderTotal();
    }
    ImGui::PopStyleColor();
}

void TradingPanel::RenderPriceAmountInputs()
{
    // Price input (only for limit and stop limit orders)
    if (m_orderType == OrderType::Limit || m_orderType == OrderType::StopLimit)
    {
        ImGui::Text("Price (%s):", m_quoteCurrency.c_str());
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputFloat("##Price", &m_price, 1.0f, 100.0f, "%.2f"))
        {
            CalculateOrderTotal();
        }
    }
    else
    {
        ImGui::Text("Price: Market (≈ $%.2f)", m_currentPrice);
    }

    // Stop price for stop orders
    if (m_orderType == OrderType::Stop || m_orderType == OrderType::StopLimit)
    {
        ImGui::Text("Stop Price (%s):", m_quoteCurrency.c_str());
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputFloat("##StopPrice", &m_stopPrice, 1.0f, 100.0f, "%.2f"))
        {
            CalculateOrderTotal();
        }
    }

    // Amount input
    ImGui::Text("Amount (%s):", m_baseCurrency.c_str());
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputFloat("##Amount", &m_amount, 0.001f, 0.1f, "%.4f"))
    {
        CalculateOrderTotal();
    }

    // Quick amount buttons (25%, 50%, 75%, 100% of available balance)
    ImGui::Text("Quick Select:");

    double availableBalance = (m_orderSide == OrderSide::Buy) ?
                             (m_quoteBalance / m_currentPrice) : m_baseBalance;

    if (ImGui::Button("25%"))
    {
        m_amount = availableBalance * 0.25f;
        CalculateOrderTotal();
    }
    ImGui::SameLine();

    if (ImGui::Button("50%"))
    {
        m_amount = availableBalance * 0.50f;
        CalculateOrderTotal();
    }
    ImGui::SameLine();

    if (ImGui::Button("75%"))
    {
        m_amount = availableBalance * 0.75f;
        CalculateOrderTotal();
    }
    ImGui::SameLine();

    if (ImGui::Button("100%"))
    {
        m_amount = availableBalance;
        CalculateOrderTotal();
    }

    // Order total
    ImGui::Separator();
    ImGui::Text("Order Total: %.2f %s", m_orderTotal, m_quoteCurrency.c_str());
}

void TradingPanel::RenderOrderButton()
{
    // Determine button color based on side
    ImVec4 buttonColor = (m_orderSide == OrderSide::Buy) ?
                        ImVec4(0.2f, 0.8f, 0.2f, 0.8f) :
                        ImVec4(0.8f, 0.2f, 0.2f, 0.8f);

    ImVec4 buttonHoverColor = (m_orderSide == OrderSide::Buy) ?
                             ImVec4(0.3f, 0.9f, 0.3f, 1.0f) :
                             ImVec4(0.9f, 0.3f, 0.3f, 1.0f);

    // Check if order is valid
    bool canPlaceOrder = m_amount > 0.0001f;
    if (m_orderType == OrderType::Limit || m_orderType == OrderType::StopLimit)
    {
        canPlaceOrder &= m_price > 0.01f;
    }
    if (m_orderType == OrderType::Stop || m_orderType == OrderType::StopLimit)
    {
        canPlaceOrder &= m_stopPrice > 0.01f;
    }

    // Check available balance
    if (m_orderSide == OrderSide::Buy)
    {
        canPlaceOrder &= m_orderTotal <= m_quoteBalance;
    }
    else
    {
        canPlaceOrder &= m_amount <= m_baseBalance;
    }

    if (!canPlaceOrder)
        ImGui::BeginDisabled();

    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoverColor);

    const char* buttonText = (m_orderSide == OrderSide::Buy) ? "BUY" : "SELL";
    if (ImGui::Button(buttonText, ImVec2(-1, 50)))
    {
        if (m_confirmOrder)
        {
            // Place the order
            OrderRequest order;
            order.type = m_orderType;
            order.side = m_orderSide;
            order.price = m_price;
            order.amount = m_amount;
            order.stopPrice = m_stopPrice;
            order.currencyPair = m_currencyPair;

            if (m_orderCallback)
            {
                m_orderCallback(order);
            }

            printf("Order placed: %s %s %.4f %s",
                   (m_orderSide == OrderSide::Buy) ? "BUY" : "SELL",
                   (m_orderType == OrderType::Market) ? "Market" :
                   (m_orderType == OrderType::Limit) ? "Limit" :
                   (m_orderType == OrderType::Stop) ? "Stop" : "Stop Limit",
                   m_amount, m_baseCurrency.c_str());

            if (m_orderType != OrderType::Market)
                printf(" at %.2f %s", m_price, m_quoteCurrency.c_str());

            printf(" (Total: %.2f %s)\n", m_orderTotal, m_quoteCurrency.c_str());

            m_confirmOrder = false;
        }
        else
        {
            m_confirmOrder = true;
        }
    }

    ImGui::PopStyleColor(2);

    if (!canPlaceOrder)
        ImGui::EndDisabled();

    // Show confirmation or validation messages
    if (!canPlaceOrder)
    {
        if (m_orderSide == OrderSide::Buy && m_orderTotal > m_quoteBalance)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
                             "Insufficient %s balance", m_quoteCurrency.c_str());
        }
        else if (m_orderSide == OrderSide::Sell && m_amount > m_baseBalance)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
                             "Insufficient %s balance", m_baseCurrency.c_str());
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Invalid order parameters");
        }
    }
    else if (m_confirmOrder)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "Click again to confirm order");

        if (ImGui::Button("Cancel"))
        {
            m_confirmOrder = false;
        }
    }
}

void TradingPanel::RenderAccountBalance()
{
    ImGui::Text("Account Balance:");

    ImGui::Text("%s: %.4f", m_baseCurrency.c_str(), m_baseBalance);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(≈ $%.2f)", m_baseBalance * m_currentPrice);

    ImGui::Text("%s: %.2f", m_quoteCurrency.c_str(), m_quoteBalance);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(≈ %.4f %s)", m_quoteBalance / m_currentPrice, m_baseCurrency.c_str());

    // Total portfolio value
    double totalValue = (m_baseBalance * m_currentPrice) + m_quoteBalance;
    ImGui::Text("Portfolio Value: $%.2f", totalValue);
}

void TradingPanel::SetCurrencyPair(const std::string& pair)
{
    m_currencyPair = pair;

    // Parse currency pair (e.g., "BTC/USD" -> "BTC", "USD")
    size_t slashPos = pair.find('/');
    if (slashPos != std::string::npos)
    {
        m_baseCurrency = pair.substr(0, slashPos);
        m_quoteCurrency = pair.substr(slashPos + 1);
    }

    CalculateOrderTotal();
}

void TradingPanel::SetBalance(double baseBalance, double quoteBalance)
{
    m_baseBalance = baseBalance;
    m_quoteBalance = quoteBalance;
}

void TradingPanel::CalculateOrderTotal()
{
    double effectivePrice = (m_orderType == OrderType::Market) ? m_currentPrice : m_price;
    m_orderTotal = m_amount * effectivePrice;
}

void TradingPanel::Shutdown()
{
    // Cleanup if needed
}
} // namespace panel
} // namespace gui