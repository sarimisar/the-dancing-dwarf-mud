/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "itemresource.h"
#include "character.h"
#include "../global.h"
#include "../globaldebug.h"
#include "world.h"

#include <QJsonArray>

using namespace CORE;



ItemResource::ItemResource(Entity *parent) :
    Item(QMUD::ItemType::RESOURCE, parent),
    m_eResource(QMUD::ResourceType::UNKNOWN),
    m_timeCollectable(QDateTime::currentDateTime()),
    m_iAmount(1)
{

}

ItemResource::~ItemResource()
{
    GLOBAL_DEBUG_DESTRUCTOR("ItemResource", "Id: " + QString::number(id()) +
                            " StaticId: " + QString::number(staticId()) +
                            " MapId: " + QString::number(mapStaticId()));
}

Item *ItemResource::create(QMUD::ResourceType resourceType, Entity *parent)
{
    QString name;
    QString description;
    QString action;
    QString actionOther;
    int amount;

    switch (resourceType)
    {
    case QMUD::ResourceType::GARLIC:
        name = "una piantina d'aglio selvatico";
        description = "Una piccola piantina di aglio selvatico spunta dal terreno";
        action = "Strappi dal terreno una piantina di aglio selvatico ricavandone il bulbo";
        actionOther = "%1 strappa dal terreno una piantina di aglio selvatico ricavandone il bulbo";
        amount = 2;
        break;

    case QMUD::ResourceType::MARIGOLD:
        name = "una pianta di fiori di Calendula";
        description = "Una pianta di fiori di Calendula spunta dal terreno";
        action = "Stacchi delicatamente alcuni fiori di calendula da una pianta";
        actionOther = "%1 stacca delicatamente alcuni fiori da una calendula da una pianta";
        amount = 2;
        break;

    case QMUD::ResourceType::HYSSOP:
        name = "un piccolo cespuglio di Issopo";
        description = "Un piccolo cespuglio di Issopo spunta dal terreno";
        action = "Stacchi delicatamente alcuni rametti di issopo da un cespuglio";
        actionOther = "%1 stacca delicatamente alcuni rametti di issopo da un cespuglio";
        amount = 2;
        break;

    case QMUD::ResourceType::PRIMULA:
        name = "una pianta di fiori di Primula";
        description = "Una pianta di fiori di Primula spunta dal terreno";
        action = "Strappi dal terreno una piantina di primula ricavandone la radice";
        actionOther = "%1 strappa dal terreno una piantina di primula ricavandone la radice";
        amount = 2;
        break;

    case QMUD::ResourceType::RED_CURRANT:
        name = "un piccolo cespuglio di ribes rosso";
        description = "Un piccolo cespuglio di ribes spunta dal terreno ricoperto di piccoli frutti rossi";
        action = "Stacchi delicatamente alcuni ribes rossi da un piccolo cespuglio";
        actionOther = "%1 stacca delicatamente alcuni ribes rossi da un piccolo cespuglio";
        amount = 4;
        break;

    case QMUD::ResourceType::GOOSEBERRY:
        name = "un cespuglio di uva spina";
        description = "Un cespuglio di uva spina spunta dal terreno ricoperto di piccole bacche";
        action = "Stacchi delicatamente alcune bacche di uva spina";
        actionOther = "%1 stacca delicatamente alcune bacche di uva spina da un cespuglio";
        amount = 4;
        break;

    case QMUD::ResourceType::GINGER:
        name = "una pianta di zenzero";
        description = "Una pianta di zenzero spunta dal terreno";
        action = "Strappi dal terreno una pianta di zenzero ricavandone la radice";
        actionOther = "%1 strappa dal terreno una pianta di zenzero ricavandone la radice";
        amount = 2;
        break;

    case QMUD::ResourceType::LAVENDER:
        name = "una pianta di fiori di lavanda";
        description = "Una pianta di fiori di lavanda spunta dal terreno";
        action = "Stacchi alcuni rametti di lavanda dalla pianta";
        actionOther = "%1 stacca delicatamente alcuni rametti di lavanda da una pianta";
        amount = 2;
        break;

    case QMUD::ResourceType::POPPY:
        name = "delle piantine di papavero";
        description = "Alcuni piantine di papavero spuntano dal terreno";
        action = "Strappi delicatamente alcuni fiori di papavero dal terreno";
        actionOther = "%1 strappa delicatamente alcuni fiori di papavero dal terreno";
        amount = 2;
        break;

    case QMUD::ResourceType::BLACK_CURRANT:
        name = "un piccolo cespuglio di ribes nero";
        description = "Un piccolo cespuglio di ribes spunta dal terreno ricoperto di piccoli frutti neri";
        action = "Stacchi delicatamente alcuni ribes neri da un piccolo cespuglio";
        actionOther = "%1 stacca delicatamente alcuni ribes neri da un piccolo cespuglio";
        amount = 4;
        break;

    case QMUD::ResourceType::TILIA:
        name = "un albero di Tiglio";
        description = "Un albero di Tiglio";
        action = "Stacchi alcuni pezzi di corteccia dall'albero";
        actionOther = "%1 stacca alcuni pezzi di corteccia da un albero di tiglio";
        amount = 2;
        break;

    case QMUD::ResourceType::HOP:
        name = "una pianta di Luppolo";
        description = "Una pianta di Luppolo fiorita";
        action = "Stacchi delicatamente alcuni coni di luppolo dalla pianta";
        actionOther = "%1 stacca delicatamente alcuni coni di luppolo da una pianta";
        amount = 2;
        break;

    case QMUD::ResourceType::WITCH_HAZEL:
        name = "una pianta di Amamelide";
        description = "Una pianta di Amamelide fiorita";
        action = "Stacchi alcune foglie e rametti dalla pianta";
        actionOther = "%1 stacca alcune foglie e rametti da una pianta di amamelide";
        amount = 2;
        break;

    case QMUD::ResourceType::SAFFLOWER:
        name = "una pianta di Cartamo";
        description = "Una pianta di Cartamo fiorita";
        action = "Stacchi alcuni fiori di cartamo dalla pianta";
        actionOther = "%1 stacca alcuni fiori da una pianta di cartamo";
        amount = 2;
        break;

    case QMUD::ResourceType::GINKGO:
        name = "una albero di Ginkgo";
        description = "Un albero di Ginkgo biloba";
        action = "Stacchi alcuni dei rametti piu' bassi dall'albero";
        actionOther = "%1 stacca alcuni dei rametti piu' bassi di un albero di Ginkgo";
        amount = 2;
        break;

    case QMUD::ResourceType::VERBENA:
        name = "una pianta di Verbena";
        description = "Un pianta di Verbena officinalis";
        action = "Stacchi alcuni rametti dalla pianta";
        actionOther = "%1 stacca alcuni rametti da una pianta di verbena";
        amount = 2;
        break;

    case QMUD::ResourceType::ANGELICA:
        name = "una pianta di Angelica";
        description = "Un pianta di Angelica keiskei";
        action = "Strappi dal terreno alcuni pezzi di radice della pianta";
        actionOther = "%1 strappa dal terreno alcuni pezzi di radice di una pianta di Angelica";
        amount = 2;
        break;

    case QMUD::ResourceType::ANISE:
        name = "una pianta di anice";
        description = "Un pianta di Anice verde";
        action = "Stacchi alcuni grappoli dalla pianta di anice";
        actionOther = "%1 stacca alcuni grappoli da una pianta di anice";
        amount = 3;
        break;

    case QMUD::ResourceType::EDELWEISS:
        name = "una piantina di Leontopodium";
        description = "Un pianta di Leontopodium dai fiori stellati";
        action = "Stacchi alcuni fiori stellati dalla piantina";
        actionOther = "%1 stacca alcuni fiori stellati da una piantina di Lentopodium";
        amount = 1;
        break;

    case QMUD::ResourceType::HELICHRYSUM:
        name = "un cespuglio di Elicriso";
        description = "Un cespuglio di Elicriso fiorito";
        action = "Stacchi alcuni rametti fioriti dal cespuglio";
        actionOther = "%1 stacca alcuni rametti fioriti da un cespuglio di Elicriso";
        amount = 3;
        break;

    case QMUD::ResourceType::HOLLY:
        name = "un arbusto di Pungitopo";
        description = "Un arbusto di Pungitopo ricco di bacche e piccoli aghi";
        action = "Stacchi alcuni germogli dal piccolo albero";
        actionOther = "%1 stacca alcuni germogli da un piccolo albero di Pungitopo";
        amount = 1;
        break;

    case QMUD::ResourceType::ILEX:
        name = "un piccolo albero di Agrifoglio";
        description = "Un piccolo albero di Agrifoglio";
        action = "Stacchi alcuni pezzi di corteccia e rametti dall'albero";
        actionOther = "%1 stacca alcuni pezzi di corteccia e rametti da un piccolo albero di Agrifoglio";
        amount = 1;
        break;

    case QMUD::ResourceType::ELDER:
        name = "un albero di Sambuco";
        description = "Un albero di Sambuco alto poco meno di dieci metri";
        action = "Stacchi alcuni fiori dai rami piu' bassi dell'abero";
        actionOther = "%1 stacca alcuni fiori dai rami piu' bassi di un albero di Sambuco";
        amount = 3;
        break;

    case QMUD::ResourceType::FOXGLOVE:
        name = "una pianta di Digitalis purpurea";
        description = "Una pianta piena di fiori a calice";
        action = "Stacchi alcune foglie dalla pianta";
        actionOther = "%1 stacca alcune foglie da una pianta di Digitalis purpurea";
        amount = 3;
        break;

    case QMUD::ResourceType::AGAVE:
        name = "una pianta di Agave";
        description = "Una grossa pianta piena di foglie carnose";
        action = "Stacchi alcune foglie carnose dalla pianta";
        actionOther = "%1 stacca alcune foglie carnose da una pianta di Agave";
        amount = 2;
        break;

    case QMUD::ResourceType::AMBROSIA:
        name = "una pianta di Ambrosia";
        description = "Una pianta di Ambrosia";
        action = "Stacchi alcune foglie dalla pianta";
        actionOther = "%1 stacca alcune foglie da una pianta di Ambrosia";
        amount = 2;
        break;

    case QMUD::ResourceType::SUNDEW:
        name = "una pianta carnivora di Drosera rotundifolia";
        description = "Una pianta carnivora di Drosera rotundifolia";
        action = "Stacchi alcune foglie dalla pianta";
        actionOther = "%1 stacca alcune foglie da una pianta di Drosera rotundifolia";
        amount = 2;
        break;

    case QMUD::ResourceType::HAWTHORN:
        name = "una pianta di Biancospino";
        description = "Una pianta di Biancospino coperta di bacche rosse e fiori bianchi";
        action = "Stacchi alcune bacche rosse dalla pianta";
        actionOther = "%1 stacca alcune bacche rosse da una pianta di biancospino";
        amount = 3;
        break;

    case QMUD::ResourceType::LOTUS:
        name = "una piccola piantina di Fior di loto";
        description = "Una piccola piantina di Fior di loto che galleggia in un piccolo ristagno d'acqua";
        action = "Strappi alcune foglie e petali dalla pianta";
        actionOther = "%1 stacca alcune foglie e petali da una pianta di Fior di loto";
        amount = 1;
        break;

    case QMUD::ResourceType::MORINGA:
        name = "un albero di Moringa";
        description = "Un albero di Moringa";
        action = "Stacchi alcuni baccelli dall'albero";
        actionOther = "%1 stacca alcuni baccelli da un albero di Moringa";
        amount = 2;
        break;

    case QMUD::ResourceType::ALCHEMILLA:
        name = "una piccola piantina di Alchemilla";
        description = "Una piccola piantina di Alchemilla";
        action = "Stacchi alcune foglie dalla pianta";
        actionOther = "%1 stacca alcune foglie da una pianta di Alchemilla";
        amount = 3;
        break;

    case QMUD::ResourceType::ROBINIA:
        name = "un grande albero di Robinia";
        description = "Una grande albero di Robinia";
        action = "Stacchi alcune bacche dai rami piu' bassi dell'albero";
        actionOther = "%1 stacca alcune bacche dai rami piu' bassi di un albero di Robinia";
        amount = 3;
        break;

    case QMUD::ResourceType::ESCOLZIA:
        name = "una pianta di Escolzia";
        description = "Una pianta di Escolzia fiorita";
        action = "Stacchi delicatamente alcune sommita' fiorite dalla pianta";
        actionOther = "%1 stacca delicatamente alcune sommita' fiorite da una pianta di Escolzia";
        amount = 2;
        break;

    case QMUD::ResourceType::NETTLE:
        name = "una piccola pianta di Ortica";
        description = "Una piccola pianta di Ortica";
        action = "Stacchi alcune foglie dalla pianta con grande attenzione";
        actionOther = "%1 stacca con grande attenzione alcune foglie da una pianta di ortica";
        amount = 2;
        break;

    case QMUD::ResourceType::STRAMONIUM:
        name = "una pianta di Stramonio";
        description = "Una pianta di Stramonio, l'erba delle streghe";
        action = "Stacchi alcune foglie dalla pianta";
        actionOther = "%1 stacca alcune foglie da una pianta di Stramonio";
        amount = 2;
        break;

    case QMUD::ResourceType::COPPER:
    case QMUD::ResourceType::TIN:
    case QMUD::ResourceType::IRON:
    case QMUD::ResourceType::ZINC:
    case QMUD::ResourceType::ANTIMONY:
    case QMUD::ResourceType::ALUMINIUM:
    case QMUD::ResourceType::CHROME:
    case QMUD::ResourceType::LEAD:
    case QMUD::ResourceType::CADMIUM:
    case QMUD::ResourceType::SILVER:
    case QMUD::ResourceType::NICKEL:
    case QMUD::ResourceType::BISMUTH:
    case QMUD::ResourceType::GOLD:
    case QMUD::ResourceType::TITANIUM:
    case QMUD::ResourceType::PLATINUM:
    case QMUD::ResourceType::IRIDIUM:
    case QMUD::ResourceType::RHODIUM:
    case QMUD::ResourceType::OSMIUM:
    case QMUD::ResourceType::BRONZE:
    case QMUD::ResourceType::BRASS:
    case QMUD::ResourceType::PEWTER:
    case QMUD::ResourceType::DURALLUMINIUM:
    case QMUD::ResourceType::ALPACCA:
    case QMUD::ResourceType::NICROSIL:
    case QMUD::ResourceType::CORROBEND:
    case QMUD::ResourceType::ELECTRUM:
    case QMUD::ResourceType::PLATINUM_MAX:
    case QMUD::ResourceType::LUCENS:
    case QMUD::ResourceType::ALTIUM:
        name = "una cava di " + QMUD::resourceToReadableString(resourceType);
        description = "Una cava di " + QMUD::resourceToReadableString(resourceType);
        action = "Estrai " + QMUD::resourceToReadableString(resourceType) + " dalla cava";
        actionOther = "%1 estrae " + QMUD::resourceToReadableString(resourceType) + " da una cava";
        amount = 3;
        break;

    default:
        return Q_NULLPTR;
    }

    ItemResource* item = new ItemResource(Q_NULLPTR);
    item->setParent(parent);
    item->m_iStaticId = QMUD_GLOBAL_STATIC_ID_ITEM_RESOURCE;
    item->m_iMapStaticId = QMUD_GLOBAL_MAP_STATIC_ID_ITEM_RESOURCE;
    item->m_strName = name;
    item->m_strDescription = description;
    item->m_eResource = resourceType;
    item->m_strAction = action;
    item->m_strActionOther = actionOther;
    item->m_iAmount = amount;

    return item;
}

QMUD::ResourceType CORE::ItemResource::resource() const
{
    return m_eResource;
}

bool ItemResource::collectable(QDateTime now) const
{
    return m_timeCollectable <= now;
}

bool ItemResource::collectable() const
{
    return m_timeCollectable <= QDateTime::currentDateTime();
}

int ItemResource::collect()
{
    m_timeCollectable = QDateTime::currentDateTime().addSecs(QMUD_GLOBAL_ITEM_RESOURCE_RESET_SEC);
    return QMUD::rollDice(1, m_iAmount);
}

QString ItemResource::action() const
{
    return m_strAction;
}

QString ItemResource::actionOther() const
{
    return m_strActionOther;
}
