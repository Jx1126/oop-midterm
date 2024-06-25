#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "Candlestick.h"

MerkelMain::MerkelMain()
{
}

void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);

    while (true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}

void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 Midterm task 1 - Print trading data
    std::cout << "6: Print requested trading data [Task 1] " << std::endl;
    // 7 Midterm task 2 - Exporting candlestick graph as txt
    std::cout << "7: Export candlestick graph as txt [Task 2]" << std::endl;
    // 8 Midterm task 3 - Plotting a graph
    std::cout << "8: Export USDT/USD data as candlestick graph as txt [Task 3] " << std::endl;
    // 9 continue
    std::cout << "9: Continue " << std::endl;

    std::cout << "============== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (std::string const &p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                                                  p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    }
    // std::cout << "OrderBook contains :  " << orders.size() << " entries" << std::endl;
    // unsigned int bids = 0;
    // unsigned int asks = 0;
    // for (OrderBookEntry& e : orders)
    // {
    //     if (e.orderType == OrderBookType::ask)
    //     {
    //         asks ++;
    //     }
    //     if (e.orderType == OrderBookType::bid)
    //     {
    //         bids ++;
    //     }
    // }
    // std::cout << "OrderBook asks:  " << asks << " bids:" << bids << std::endl;
}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else
    {
        try
        {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::ask);
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else
            {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else
    {
        try
        {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::bid);
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else
            {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}

void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry &sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl;
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
    }

    currentTime = orderBook.getNextTime(currentTime);
}

int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-9" << std::endl;
    std::getline(std::cin, line);
    try
    {
        userOption = std::stoi(line);
    }
    catch (const std::exception &e)
    {
        //
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        std::cout << "Invalid choice. Choose 1-6" << std::endl;
    }
    if (userOption == 1)
    {
        printHelp();
    }
    if (userOption == 2)
    {
        printMarketStats();
    }
    if (userOption == 3)
    {
        enterAsk();
    }
    if (userOption == 4)
    {
        enterBid();
    }
    if (userOption == 5)
    {
        printWallet();
    }
    if (userOption == 6)
    {
        printTradingData();
    }
    if (userOption == 7)
    {
        generateCandlestick();
    }
    if (userOption == 8)
    {
        generateGraph();
    }
    if (userOption == 9)
    {
        gotoNextTimeframe();
    }
}

//START
// Function to print the trading data
void MerkelMain::printTradingData()
{
    std::cout << "Print trading data - enter the following values: 'product/product,type', eg: 'ETH/BTC,ask' " << std::endl;
    // Get the user input as a string
    std::string user_input;
    // Get the user input
    std::getline(std::cin, user_input);

    // Tokenizing the user input when a "," is found
    std::vector<std::string> tokens = CSVReader::tokenise(user_input, ',');
    if (tokens.size() != 2)
    {
        std::cout << "Bad input, please enter following the format: 'product/product,type' " << std::endl;
    }
    else
    {
        try
        {
            // Getting the trading data and storing it in a vector
            std::map<TimeSystem, std::vector<OrderBookEntry>> joint_data = Candlestick::jointGroup("20200601.csv", tokens[0], OrderBookEntry::stringToOrderBookType(tokens[1]));

            // Combining the data
            std::vector<Candlestick> combined_data = Candlestick::combineData(joint_data);

            // Printing the user request
            std::cout << "Requested candlestick data for " << tokens[0] << " and " << tokens[1] << ": " << std::endl;

            // Printing the trading data
            for (const auto &candlestick : combined_data)
            {
                std::cout << "Timestamp: " << candlestick.timestamp << " | "
                          << "Open: " << candlestick.open << " | "
                          << "Close: " << candlestick.close << " | "
                          << "Low: " << candlestick.low << " | "
                          << "High: " << candlestick.high << " | "
                          << "Market: " << candlestick.market << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "Bad input, please enter following the format: 'product/product,type' " << std::endl;
        }
    }
}

void MerkelMain::generateCandlestick()
{

    std::cout << "Generate candlestick graph - enter the following values: 'product/product,type', eg: 'ETH/BTC,ask' " << std::endl;

    // Get the user input as a string
    std::string user_input;

    // Get the user input
    std::getline(std::cin, user_input);

    // Tokenizing the user input when a "," is found
    std::vector<std::string> tokens = CSVReader::tokenise(user_input, ',');
    // If the tokenized input is not equal to 2
    if (tokens.size() != 2)
    {
        // Print out the following message
        std::cout << "Bad input, please enter following the format: 'product/product,type' " << std::endl;
    }
    else
    {
        try
        {
            // Getting the trading data and storing it in a vector
            std::map<TimeSystem, std::vector<OrderBookEntry>> joint_data = Candlestick::jointGroup("20200601.csv", tokens[0], OrderBookEntry::stringToOrderBookType(tokens[1]));

            // Combining the data
            std::vector<Candlestick> combined_data = Candlestick::combineData(joint_data);

            // Printing the user request
            std::cout << "Requested candlestick data for " << tokens[0] << " and " << tokens[1] << ": " << std::endl;

            // Printing the trading data
            std::vector<std::string> productToken = CSVReader::tokenise(tokens[0], '/');

            // Renaming the file
            std::string rename_file = productToken[0] + "_" + productToken[1] + "_" + tokens[1] + ".txt";

            std::cout << "Candlestick graph has been exported as " << rename_file << std::endl;


            // Calling the function to plot the candlestick graph
            Candlestick::drawCandlestickGraph(combined_data, rename_file);
        }
        catch (const std::exception &e)
        {
            std::cout << "Bad input, please enter following the format: 'product/product,type' " << std::endl;
        }
    }
}

// Generating task 3 candlestick graph
void MerkelMain::generateGraph()
{
    // Reading the csv file and storing it in a vector
    std::vector<Candlestick> candlesticks = CSVReader::candlestickReader("task_3.csv");

    // Renaming the file
    std::string rename_file = "task_3.txt";

    // Calling the function to plot the candlestick graph and export the txt file name
    Candlestick::drawCandlestickGraph(candlesticks, rename_file);

    std::cout << "Candlestick graph has been exported as " << rename_file << std::endl;

    // Printing the candlestick graph
    for (const Candlestick &cs : candlesticks)
    {
        // Printing the data
        std::cout << "Timestamp: " << cs.timestamp << " | "
                  << "Open: " << cs.open << " | "
                  << "High: " << cs.close << " | "
                  << "Low: " << cs.low << " | "
                  << "Close: " << cs.high << " | "
                  << "Market: " << cs.market << std::endl;
        std::cout << "=================" << std::endl;
    }
}
//END