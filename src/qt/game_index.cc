#include "game_index.h"
#include "common.h"

namespace retropia
{

    GameIndex::GameIndex(const QString &fileName, unsigned int nThreads, QObject *parent) :
        QObject(parent),
        m_gameIndex(NULL)
    {
        QByteArray ba = fileName.toLocal8Bit();
        m_gameIndex = r_game_index_create(ba.constData(), nThreads);

        if (!m_gameIndex)
        {
            R_RUNTIME_ERROR();
        }
        else
        {
            r_game_index_on_refresh(m_gameIndex, GameIndex::onRefreshCallback, this);
        }
    }

    GameIndex::~GameIndex(void)
    {
        if (m_gameIndex)
        {
            r_game_index_destroy(m_gameIndex);
        }
    }

    void GameIndex::addSearchPath(const QString& path)
    {
        QByteArray ba = path.toLocal8Bit();
        r_game_index_add_search_path(m_gameIndex, ba.constData());
    }

    void GameIndex::removeSearchPath(const QString& path)
    {
        QByteArray ba = path.toLocal8Bit();
        r_game_index_remove_search_path(m_gameIndex, ba.constData());
    }

    bool GameIndex::refresh(void)
    {
        return (r_game_index_refresh(m_gameIndex) != 0);
    }

    void GameIndex::emitOnRefresh(const GameInfo &info) const
    {
        emit onRefresh(info);
    }

    void GameIndex::onRefreshCallback(const r_game_info_t info, void *obj)
    {
        GameIndex *gameIndex = (GameIndex *) obj;

        GameInfo gameInfo(info);
        gameIndex->emitOnRefresh(gameInfo);
    }
}
