#include "StateTestFillerExpectSection.h"
#include <TestHelper.h>

#include <retesteth/Options.h>
namespace test
{
namespace teststruct
{
StateTestFillerExpectSection::StateTestFillerExpectSection(DataObject const& _data)
{
    parseJsonIntValueIntoSet(_data.atKey("indexes").atKey("data"), m_dataInd);
    parseJsonIntValueIntoSet(_data.atKey("indexes").atKey("gas"), m_gasInd);
    parseJsonIntValueIntoSet(_data.atKey("indexes").atKey("value"), m_valInd);

    // get allowed networks for this expect section
    parseJsonStrValueIntoSet(_data.atKey("network"), m_forks);

    // Parse >=Frontier into  Frontier, Homestead, ... Constantinople according to current config
    ClientConfig const& cfg = Options::get().getDynamicOptions().getCurrentConfig();
    bool isTransitionTest = false;
    if (m_forks.size() == 1)
    {
        for (auto const& net : m_forks)
            if (inArray(cfg.getAdditionalNetworks(), net))
                isTransitionTest = true;
    }
    if (!isTransitionTest)
        m_forks = translateNetworks(m_forks, cfg.getNetworks());

    // Convert Expect Section IncompletePostState fields to hex
    DataObject tmpD = _data.atKey("result");
    if (tmpD.count("storage"))
        tmpD.atKeyUnsafe("storage").performModifier(mod_keyToCompactEvenHexPrefixed);
    tmpD.performModifier(mod_valueToCompactEvenHexPrefixed);

    m_result = GCP_SPointer<StateIncomplete>(new StateIncomplete(tmpD));
}

}  // namespace teststruct
}  // namespace test