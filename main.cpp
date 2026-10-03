#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

// ================= TRANSACTION STRUCT =================

struct Transaction
{
    string symbol;
    string type; // "BUY" or "SELL"
    int quantity;
    double price;
    double total;
};

// ================= STOCK CLASS =================

class Stock
{
private:
    string symbol;
    string name;
    double price;
    int quantity;

public:
    Stock(string s, string n, double p)
        : symbol(s), name(n), price(p), quantity(0) {
    }

    string getSymbol() const { return symbol; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void buy(int qty)
    {
        quantity += qty;
    }

    bool sell(int qty)
    {
        if (quantity >= qty && qty > 0)
        {
            quantity -= qty;
            return true;
        }
        return false;
    }

    double getValue() const
    {
        return price * quantity;
    }
};

// ================= BUTTON CLASS =================

class Button
{
private:
    sf::RectangleShape shape;
    sf::Text text;

public:
    Button(const sf::Font& font, const string& label)
        : shape({ 0.f, 0.f }), text(font, label, 18)
    {
        shape.setFillColor(sf::Color(60, 120, 200));
        text.setFillColor(sf::Color::White);
    }

    void updateBounds(sf::Vector2f position, sf::Vector2f size)
    {
        shape.setPosition(position);
        shape.setSize(size);

        sf::FloatRect bounds = text.getLocalBounds();
        text.setPosition({
            position.x + (size.x - bounds.size.x) / 2.f - bounds.position.x,
            position.y + (size.y - bounds.size.y) / 2.f - bounds.position.y
            });
    }

    bool contains(sf::Vector2f mousePosition) const
    {
        return shape.getGlobalBounds().contains(mousePosition);
    }

    void draw(sf::RenderWindow& window) const
    {
        window.draw(shape);
        window.draw(text);
    }
};

// ================= MAIN =================

int main()
{
    // ---------- FONT LOAD WITH FALLBACKS ----------

    sf::Font font;
    if (!font.openFromFile("arial.ttf") &&
        !font.openFromFile("C:/Windows/Fonts/arial.ttf") &&
        !font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
    {
        return 1;
    }

    // ---------- FULLSCREEN WINDOW ----------

    sf::RenderWindow window(
        sf::VideoMode::getDesktopMode(),
        "Portfolio Management System",
        sf::State::Fullscreen // Note: Use sf::Style::Fullscreen for SFML 2.x
    );

    window.setFramerateLimit(60);

    // ---------- STOCKS ----------

    vector<Stock> stocks =
    {
        Stock("NVDA", "NVIDIA", 177.00),
        Stock("AMD",  "Advanced Micro Devices", 155.00),
        Stock("AAPL", "Apple", 238.00),
        Stock("MSFT", "Microsoft", 500.00),
        Stock("AMZN", "Amazon", 230.00),
        Stock("GOOGL", "Alphabet", 230.00),
        Stock("META", "Meta Platforms", 750.00),
        Stock("TSLA", "Tesla", 350.00),
        Stock("NFLX", "Netflix", 1200.00),
        Stock("AVGO", "Broadcom", 350.00),
        Stock("JPM", "JPMorgan Chase", 290.00),
        Stock("WMT", "Walmart", 100.00),
        Stock("DIS", "Disney", 115.00),
        Stock("INTC", "Intel", 25.00),
        Stock("ORCL", "Oracle", 250.00),
        Stock("BA", "Boeing", 230.00),
        Stock("SPX", "S&P 500 Index", 6500.00),
        Stock("CRM", "Salesforce", 250.00),
        Stock("ADBE", "Adobe", 350.00),
        Stock("QCOM", "Qualcomm", 170.00),
        Stock("TXN", "Texas Instruments", 210.00),
        Stock("INTU", "Intuit", 650.00),
        Stock("IBM", "IBM", 280.00),
        Stock("CSCO", "Cisco Systems", 70.00),
        Stock("UBER", "Uber", 95.00),
        Stock("SHOP", "Shopify", 140.00),
        Stock("SQ", "Block", 75.00),
        Stock("PLTR", "Palantir", 150.00)
    };

    // ---------- STATE VARIABLES ----------

    int selectedStock = 0;
    string quantityInput = "1";
    string statusMessage = "Select a stock, enter quantity, and click BUY or SELL. Press ESC to exit.";

    vector<Transaction> transactionHistory;

    // ---------- BUTTONS ----------

    Button buyButton(font, "BUY");
    Button sellButton(font, "SELL");

    // ---------- UI TEXT OBJECTS ----------

    sf::Text title(font, "PORTFOLIO MANAGEMENT SYSTEM", 28);
    title.setFillColor(sf::Color::Black);

    sf::Text instruction(font, "Click a stock to select it | Press ESC to Exit Fullscreen", 16);
    instruction.setFillColor(sf::Color(70, 70, 70));

    sf::Text quantityLabel(font, "Quantity:", 18);
    quantityLabel.setFillColor(sf::Color::Black);

    sf::RectangleShape quantityBox;
    quantityBox.setFillColor(sf::Color::White);
    quantityBox.setOutlineColor(sf::Color::Black);
    quantityBox.setOutlineThickness(2.f);

    sf::Text status(font, "", 16);
    status.setFillColor(sf::Color(30, 100, 30));

    // ---------- MAIN LOOP ----------

    while (window.isOpen())
    {
        float screenW = static_cast<float>(window.getSize().x);
        float screenH = static_cast<float>(window.getSize().y);

        float tableWidth = screenW * 0.58f;
        float rightPanelX = screenW * 0.64f;

        // Column X positions
        float colSymbol = 40.f;
        float colName = 140.f;
        float colPrice = 400.f;
        float colQty = 520.f;
        float colVal = 620.f;

        while (const optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // ================= KEYBOARD INPUT =================

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }

                if (key->code == sf::Keyboard::Key::Backspace)
                {
                    if (!quantityInput.empty())
                        quantityInput.pop_back();
                }

                if (key->code == sf::Keyboard::Key::Enter)
                {
                    if (quantityInput.empty())
                        quantityInput = "1";
                }
            }

            // ================= TEXT INPUT =================

            if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
            {
                char character = static_cast<char>(textEntered->unicode);

                if (character >= '0' && character <= '9')
                {
                    if (quantityInput == "0")
                        quantityInput.clear();

                    quantityInput += character;
                }
            }

            // ================= MOUSE INPUT =================

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f mousePos(
                        static_cast<float>(mouse->position.x),
                        static_cast<float>(mouse->position.y)
                    );

                    int qty = quantityInput.empty() ? 0 : stoi(quantityInput);

                    // BUY
                    if (buyButton.contains(mousePos))
                    {
                        if (qty > 0)
                        {
                            stocks[selectedStock].buy(qty);
                            double totalCost = stocks[selectedStock].getPrice() * qty;

                            transactionHistory.push_back({
                                stocks[selectedStock].getSymbol(),
                                "BUY",
                                qty,
                                stocks[selectedStock].getPrice(),
                                totalCost
                                });

                            statusMessage = "Bought " + to_string(qty) + " share(s) of " + stocks[selectedStock].getSymbol();
                        }
                    }
                    // SELL
                    else if (sellButton.contains(mousePos))
                    {
                        if (qty > 0 && stocks[selectedStock].sell(qty))
                        {
                            double totalRevenue = stocks[selectedStock].getPrice() * qty;

                            transactionHistory.push_back({
                                stocks[selectedStock].getSymbol(),
                                "SELL",
                                qty,
                                stocks[selectedStock].getPrice(),
                                totalRevenue
                                });

                            statusMessage = "Sold " + to_string(qty) + " share(s) of " + stocks[selectedStock].getSymbol();
                        }
                        else
                        {
                            statusMessage = "Cannot sell " + to_string(qty) + " shares (Insufficient quantity owned).";
                        }
                    }
                    // STOCK SELECTION
                    else
                    {
                        for (size_t i = 0; i < stocks.size(); i++)
                        {
                            float y = 127.f + i * 32.f;

                            sf::FloatRect stockArea(
                                { 30.f, y },
                                { tableWidth, 30.f }
                            );

                            if (stockArea.contains(mousePos))
                            {
                                selectedStock = static_cast<int>(i);
                                statusMessage = "Selected " + stocks[i].getSymbol();
                            }
                        }
                    }
                }
            }
        }

        // ================= RENDERING =================

        window.clear(sf::Color(235, 235, 235));

        // Header Titles
        title.setPosition({ 30.f, 20.f });
        instruction.setPosition({ 30.f, 65.f });
        window.draw(title);
        window.draw(instruction);

        // ---------- TABLE HEADER ----------

        sf::RectangleShape header({ tableWidth, 32.f });
        header.setPosition({ 30.f, 95.f });
        header.setFillColor(sf::Color(40, 40, 40));
        window.draw(header);

        sf::Text hSymbol(font, "SYMBOL", 15);
        sf::Text hName(font, "NAME", 15);
        sf::Text hPrice(font, "PRICE", 15);
        sf::Text hQty(font, "QTY", 15);
        sf::Text hVal(font, "VALUE", 15);

        hSymbol.setPosition({ colSymbol, 101.f });
        hName.setPosition({ colName, 101.f });
        hPrice.setPosition({ colPrice, 101.f });
        hQty.setPosition({ colQty, 101.f });
        hVal.setPosition({ colVal, 101.f });

        hSymbol.setFillColor(sf::Color::White);
        hName.setFillColor(sf::Color::White);
        hPrice.setFillColor(sf::Color::White);
        hQty.setFillColor(sf::Color::White);
        hVal.setFillColor(sf::Color::White);

        window.draw(hSymbol);
        window.draw(hName);
        window.draw(hPrice);
        window.draw(hQty);
        window.draw(hVal);

        // ---------- STOCK LIST ROWS ----------

        for (size_t i = 0; i < stocks.size(); i++)
        {
            float y = 127.f + i * 32.f;

            sf::RectangleShape row({ tableWidth, 30.f });
            row.setPosition({ 30.f, y });

            if (static_cast<int>(i) == selectedStock)
                row.setFillColor(sf::Color(180, 220, 255));
            else
                row.setFillColor(sf::Color::White);

            row.setOutlineColor(sf::Color(210, 210, 210));
            row.setOutlineThickness(1.f);

            window.draw(row);

            sf::Text txtSymbol(font, stocks[i].getSymbol(), 14);
            sf::Text txtName(font, stocks[i].getName(), 14);

            ostringstream pStream, qStream, vStream;
            pStream << "$" << fixed << setprecision(2) << stocks[i].getPrice();
            qStream << stocks[i].getQuantity();
            vStream << "$" << fixed << setprecision(2) << stocks[i].getValue();

            sf::Text txtPrice(font, pStream.str(), 14);
            sf::Text txtQty(font, qStream.str(), 14);
            sf::Text txtVal(font, vStream.str(), 14);

            txtSymbol.setPosition({ colSymbol, y + 6.f });
            txtName.setPosition({ colName, y + 6.f });
            txtPrice.setPosition({ colPrice, y + 6.f });
            txtQty.setPosition({ colQty, y + 6.f });
            txtVal.setPosition({ colVal, y + 6.f });

            txtSymbol.setFillColor(sf::Color::Black);
            txtName.setFillColor(sf::Color::Black);
            txtPrice.setFillColor(sf::Color::Black);
            txtQty.setFillColor(sf::Color::Black);
            txtVal.setFillColor(sf::Color::Black);

            window.draw(txtSymbol);
            window.draw(txtName);
            window.draw(txtPrice);
            window.draw(txtQty);
            window.draw(txtVal);
        }

        // ---------- RIGHT SIDE CONTROLS PANEL ----------

        sf::Text selectedText(
            font,
            "Selected: " + stocks[selectedStock].getSymbol() + " - " + stocks[selectedStock].getName(),
            20
        );
        selectedText.setPosition({ rightPanelX, 95.f });
        selectedText.setFillColor(sf::Color::Black);
        window.draw(selectedText);

        // Quantity Box
        quantityLabel.setPosition({ rightPanelX, 140.f });
        quantityBox.setPosition({ rightPanelX, 170.f });
        quantityBox.setSize({ 200.f, 40.f });

        sf::Text quantityText(font, quantityInput, 20);
        quantityText.setPosition({ rightPanelX + 15.f, 176.f });
        quantityText.setFillColor(sf::Color::Black);

        window.draw(quantityLabel);
        window.draw(quantityBox);
        window.draw(quantityText);

        // Buttons
        buyButton.updateBounds({ rightPanelX, 225.f }, { 120.f, 45.f });
        sellButton.updateBounds({ rightPanelX + 140.f, 225.f }, { 120.f, 45.f });

        buyButton.draw(window);
        sellButton.draw(window);

        // Portfolio Total
        double totalPortfolioValue = 0;
        for (const Stock& stock : stocks)
        {
            totalPortfolioValue += stock.getValue();
        }

        ostringstream portStream;
        portStream << "Total Portfolio Value: $" << fixed << setprecision(2) << totalPortfolioValue;

        sf::Text portfolioValue(font, portStream.str(), 20);
        portfolioValue.setPosition({ rightPanelX, 290.f });
        portfolioValue.setFillColor(sf::Color(20, 80, 160));
        window.draw(portfolioValue);

        // ---------- TRANSACTION HISTORY LOGS ----------

        sf::Text logTitle(font, "TRANSACTION LOGS", 18);
        logTitle.setPosition({ rightPanelX, 340.f });
        logTitle.setFillColor(sf::Color::Black);
        window.draw(logTitle);

        sf::RectangleShape logBox({ screenW - rightPanelX - 40.f, 260.f });
        logBox.setPosition({ rightPanelX, 370.f });
        logBox.setFillColor(sf::Color(250, 250, 250));
        logBox.setOutlineColor(sf::Color(180, 180, 180));
        logBox.setOutlineThickness(1.f);
        window.draw(logBox);

        if (transactionHistory.empty())
        {
            sf::Text noLog(font, "No transactions recorded yet.", 14);
            noLog.setPosition({ rightPanelX + 15.f, 385.f });
            noLog.setFillColor(sf::Color::Black);
            window.draw(noLog);
        }
        else
        {
            // Display up to 8 most recent transactions
            int startIndex = max(0, static_cast<int>(transactionHistory.size()) - 8);
            float logY = 380.f;

            for (int i = static_cast<int>(transactionHistory.size()) - 1; i >= startIndex; i--)
            {
                const Transaction& tx = transactionHistory[i];

                ostringstream entry;
                entry << "[" << tx.type << "] "
                    << tx.quantity << " x " << tx.symbol
                    << " @ $" << fixed << setprecision(2) << tx.price
                    << " = $" << tx.total;

                sf::Text logEntry(font, entry.str(), 14);
                logEntry.setPosition({ rightPanelX + 15.f, logY });

                if (tx.type == "BUY")
                    logEntry.setFillColor(sf::Color(0, 130, 0)); // Green for buy
                else
                    logEntry.setFillColor(sf::Color(180, 0, 0)); // Red for sell

                window.draw(logEntry);
                logY += 28.f;
            }
        }

        // Status Message
        status.setPosition({ 30.f, screenH - 40.f });
        status.setString(statusMessage);
        window.draw(status);

        window.display();
    }

    return 0;
}