#include "read_mwm_task.hpp"

#include "area_shape.hpp"

#include "../std/vector.hpp"

namespace
{
  df::AreaShape * CreateFakeShape1()
  {
    df::AreaShape * shape = new df::AreaShape(Extract(0xFFEEAABB), 0.3f);
    shape->AddTriangle(m2::PointF(0.0f, 0.0f),
                       m2::PointF(1.0f, 0.0f),
                       m2::PointF(0.0f, 1.0f));

    shape->AddTriangle(m2::PointF(1.0f, 0.0f),
                       m2::PointF(0.0f, 1.0f),
                       m2::PointF(1.0f, 1.0f));
    return shape;
  }

  df::AreaShape * CreateFakeShape2()
  {
    df::AreaShape * shape = new df::AreaShape(Extract(0xFF66AAFF), 0.0f);
    shape->AddTriangle(m2::PointF(-0.5f, 0.5f),
                       m2::PointF(0.5f, 1.5f),
                       m2::PointF(0.5f, -0.5f));

    shape->AddTriangle(m2::PointF(0.5f, -0.5f),
                       m2::PointF(0.5f, 1.5f),
                       m2::PointF(1.5f, 0.5f));
    return shape;
  }
}

namespace df
{
  ReadMWMTask::ReadMWMTask(TileKey const & tileKey,
                           MemoryFeatureIndex & index,
                           EngineContext &context)
    : m_tileInfo(tileKey)
    , m_isFinished(false)
    , m_index(index)
    , m_context(context)
  {
  }

  void ReadMWMTask::Do()
  {
    if (m_tileInfo.m_featureInfo.empty())
      ReadTileIndex();

    vector<size_t> indexesToRead;
    m_index.ReadFeaturesRequest(m_tileInfo.m_featureInfo, indexesToRead);

    if (!indexesToRead.empty())
    {
      m_context.BeginReadTile(m_tileInfo.m_key);

      for (size_t i = 0; i < indexesToRead.size(); ++i)
      {
        FeatureInfo & info = m_tileInfo.m_featureInfo[i];
        ReadGeometry(info.m_id);
        info.m_isOwner = true;
      }

      m_context.EndReadTile(m_tileInfo.m_key);
    }
  }

  TileInfo const & ReadMWMTask::GetTileInfo() const
  {
    return m_tileInfo;
  }

  void ReadMWMTask::PrepareToRestart()
  {
    m_isFinished = false;
  }

  void ReadMWMTask::Finish()
  {
    m_isFinished = true;
  }

  bool ReadMWMTask::IsFinished()
  {
    return m_isFinished;
  }

  void ReadMWMTask::ReadTileIndex()
  {
    if (m_tileInfo.m_key == TileKey(-2, -1, 3))
      m_tileInfo.m_featureInfo.push_back(FeatureInfo(FeatureID(0, 1)));
    else if (m_tileInfo.m_key == TileKey(0, 1, 3))
      m_tileInfo.m_featureInfo.push_back(FeatureInfo(FeatureID(0, 2)));
    /// TODO read index specified by m_tileInfo(m_x & m_y & m_zoomLevel)
    /// TODO insert readed FeatureIDs into m_tileInfo.m_featureInfo;
  }

  void ReadMWMTask::ReadGeometry(const FeatureID & id)
  {
    if (id == FeatureID(0, 1))
      m_context.InsertShape(m_tileInfo.m_key, MovePointer<MapShape>(CreateFakeShape1()));
    else if (id == FeatureID(0, 2))
      m_context.InsertShape(m_tileInfo.m_key, MovePointer<MapShape>(CreateFakeShape2()));
    ///TODO read geometry
    ///TODO proccess geometry by styles
    ///foreach shape in shapes
    ///  m_context.InsertShape(shape);
  }
}
